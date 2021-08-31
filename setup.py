import os
import sys
import re
import shutil
import subprocess
import urllib.request
import zipfile
import tarfile

from distutils.cmd import Command
from distutils.dir_util import copy_tree, remove_tree
from setuptools.command.install import install
from setuptools.command.sdist import sdist
from setuptools import setup, find_packages



PKG_DIR = 'src/python'
PKG_NAME = 'is_msgs'
PROTO_DIR = 'src/proto'
PKG_VERSION_PATTERN = r"^[0-9]+\.[0-9]+\.[0-9]+$"
PKG_VERSION = '1.1.16'

if re.match(PKG_VERSION_PATTERN, PKG_VERSION) is None:
    raise RuntimeError(
            "Package version must matches the pattern '{}'".format(PKG_VERSION_PATTERN))

major_depencies_list = ['wheel']

for package in major_depencies_list:
    if not package in sys.modules:
        python = sys.executable
        subprocess.check_call([python, '-m', 'pip', 'install', package], stdout=subprocess.DEVNULL)

class MyZipFile(zipfile.ZipFile):
    """
        Modification necessary to keep execution permission 
        on files aftar extract *.zip files 
        
        source: https://stackoverflow.com/questions/39296101/python-zipfile-removes-execute-permissions-from-binaries
    """

    def extract(self, member, path=None, pwd=None):
        if not isinstance(member, zipfile.ZipInfo):
            member = self.getinfo(member)

        if path is None:
            path = os.getcwd()

        ret_val = self._extract_member(member, path, pwd)
        attr = member.external_attr >> 16
        os.chmod(ret_val, attr)
        return ret_val


def download_file(filename, url):
    urllib.request.urlretrieve(url,filename)

def download_protoc():

    PROTOC_VERSION = '3.6.0'
    PROTOC_ZIP_FILE = 'protoc-{}-linux-x86_64.zip'.format(PROTOC_VERSION)
    PROTOC_BASE_URL = 'https://github.com/google/protobuf/releases/download/v{}/{}'
    PROTOC_URL = PROTOC_BASE_URL.format(PROTOC_VERSION, PROTOC_ZIP_FILE)

    download_file(PROTOC_ZIP_FILE, PROTOC_URL)
    protoc_zip = MyZipFile(PROTOC_ZIP_FILE, 'r')
    protoc_zip.extractall(path='protoc')
    protoc_zip.close()


def download_protoc_gen_doc():

    PLUGIN_VERSION = '1.1.0'
    GOLANG_VERSION = '1.10'
    PLUGIN_TAR_FILE = 'protoc-gen-doc-{}.linux-amd64.go{}.tar.gz'.format(
        PLUGIN_VERSION, GOLANG_VERSION)
    PLUGIN_BASE_URL = "https://github.com/pseudomuto/protoc-gen-doc/releases/download/v{}/{}"
    PLUGIN_URL = PLUGIN_BASE_URL.format(PLUGIN_VERSION, PLUGIN_TAR_FILE)

    download_file(PLUGIN_TAR_FILE, PLUGIN_URL)
    plugin_tar = tarfile.open(name=PLUGIN_TAR_FILE, mode='r:gz')
    plugin_tar.extractall()
    plugin_tar.close()

    extracted_folder = PLUGIN_TAR_FILE.replace('.tar.gz', '')
    copy_tree(extracted_folder, 'protoc-gen-doc')
    remove_tree(extracted_folder)


class InstallWrapper(install):

    pkg_path = os.path.join(PKG_DIR, PKG_NAME)

    def run(self):
        # Run this first so the install stops in case  these fail otherwise
        # the Python package is successfully installed
        self._compile_protobufs()
        # Run the standard PyPi copy
        install.run(self)

    def _compile_protobufs(self):
        download_protoc()
        self._clean_pkg_folder()
        protos = self._transform_proto_files()
        self._run_protoc_command(PKG_DIR, protos)
        self._patch_artifacts()

    def _patch_artifacts(self):
        artifacts = self._list_pb2_artifacts()
        for artifact in artifacts:
            artifact_file_path = os.path.join(self.pkg_path, artifact)
            with open(artifact_file_path, 'r') as f:
                output = re.sub(r'([^ ]import) ([a-zA-Z]+[\w]*_pb2[ as])', r'\1 is_msgs.\2',
                                f.read())
            with open(artifact_file_path, 'w') as f:
                f.write(output)

    def _list_pb2_artifacts(self):
        return [f for f in os.listdir(self.pkg_path) if f.endswith('pb2.py')]

    def _run_protoc_command(self, base_dir, protos):
        protos_str_list = ' '.join(protos)
        protoc_command = """
        ./protoc/bin/protoc    \
            -I./protoc/include \
            --proto_path={}    \
            --python_out={} {}""".format(base_dir, base_dir, protos_str_list)
        subprocess.call(['bash', '-c', protoc_command])

    def _list_proto_files(self):
        if os.path.exists(PROTO_DIR):
            proto_files_path = os.path.join(PROTO_DIR, 'is/msgs')
        else:
            proto_files_path = os.path.join(PKG_DIR, 'is_msgs')
        protos = [f for f in os.listdir(proto_files_path) if f.endswith('.proto')]
        return protos, proto_files_path

    def _transform_proto_files(self):

        protos, proto_files_path = self._list_proto_files()
        for proto in protos:
            shutil.copy(os.path.join(proto_files_path, proto), self.pkg_path)
            proto_pkg_path = os.path.join(self.pkg_path, proto)
            with open(proto_pkg_path, 'r') as f:
                output = re.sub(r'import "is/msgs/*', r'import "is_msgs/', f.read())
            with open(proto_pkg_path, 'w') as f:
                f.write(output)

        return [os.path.join(self.pkg_path, proto) for proto in protos]

    def _clean_pkg_folder(self):
        pkg_path = os.path.join(PKG_DIR, PKG_NAME)

        old_artifacts = self._list_pb2_artifacts()
        old_proto_files = [f for f in os.listdir(pkg_path) if f.endswith('.proto')]
        for f in old_artifacts + old_proto_files:
            os.remove(os.path.join(pkg_path, f))


class ProtobufDocumentationCommand(Command):

    user_options = []
    description = "Generates protobuf's documentation"

    def initialize_options(self):
        self.doc_builder = ProtobufDocumentation()
        self.doc_builder.download_tools()

    def finalize_options(self):
        pass

    def run(self):
        self.doc_builder.build()


class ProtobufDocumentation():

    def download_tools(self):
        download_protoc()
        download_protoc_gen_doc()

    def build(self):
        proto_files_path = os.path.join(PROTO_DIR, 'is/msgs/')
        proto_files_path += "*.proto"
        protoc_command = """
        ./protoc/bin/protoc                                         \
            --plugin=protoc-gen-doc=./protoc-gen-doc/protoc-gen-doc \
            --doc_out=docs/ \
            --doc_opt=markdown,README.md \
            -I./{} \
            {}""".format(PROTO_DIR, proto_files_path)
        subprocess.call(['bash', '-c', protoc_command])


class DistributedWithDocs(sdist):
    """Custom build command."""

    def run(self):
        self.doc_builder = ProtobufDocumentation()
        self.doc_builder.download_tools()
        self.doc_builder.build()
        sdist.run(self)


setup(
    name=PKG_NAME,
    version=PKG_VERSION,
    description='Package with standard IS messages',
    url='http://github.com/labviros/is-msgs',
    author='labviros',
    license='MIT',
    package_dir={'': PKG_DIR},
    packages=[PKG_NAME, PKG_NAME + '.utils'],
    package_data={PKG_NAME: ['*.proto']},
    zip_safe=False,
    install_requires=['protobuf==3.6.0'],
    long_description_content_type='text/markdown',
    cmdclass={
        'install': InstallWrapper,
        'sdist': DistributedWithDocs,
        'build_proto_doc': ProtobufDocumentationCommand,
    },
)
