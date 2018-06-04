import os
import re
import shutil
import subprocess
import requests
import zipfile

''' 
    Modification necessary to keep execution permission 
    on files aftar extract *.zip files 
    
    source: https://stackoverflow.com/questions/39296101/python-zipfile-removes-execute-permissions-from-binaries
'''
class MyZipFile(zipfile.ZipFile):
    def extract(self, member, path=None, pwd=None):
        if not isinstance(member, zipfile.ZipInfo):
            member = self.getinfo(member)

        if path is None:
            path = os.getcwd()

        ret_val = self._extract_member(member, path, pwd)
        attr = member.external_attr >> 16
        os.chmod(ret_val, attr)
        return ret_val

''' 
    Download protoc
'''
protoc_version = '3.5.1'
protoc_zip_file = 'protoc-{}-linux-x86_64.zip'.format(protoc_version)
protoc_git_url = 'https://github.com/google/protobuf/releases/download/v{}/{}'
if not os.path.exists(protoc_zip_file):
    protoc_url = protoc_git_url.format(protoc_version, protoc_zip_file)
    with requests.get(protoc_url, stream=True) as r:
        with open(protoc_zip_file, 'wb') as f:
            for chunk in r.iter_content(chunk_size=1024):
                if chunk:
                    f.write(chunk)   
    protoc_zip = MyZipFile(protoc_zip_file, 'r')
    protoc_zip.extractall(path='protoc')
    protoc_zip.close()

''' 
    Build package files
'''
pkg_dir = 'is_msgs'

if os.path.exists(pkg_dir):
    shutil.rmtree(pkg_dir, ignore_errors=True)
os.makedirs(pkg_dir)

protos = [ f for f in os.listdir('is/msgs/') if f.endswith('.proto') ]
for proto in protos:
    shutil.copy(os.path.join('is/msgs/', proto), pkg_dir)
    with open(os.path.join(pkg_dir, proto), 'r') as f:
        output = re.sub(r'import "is/msgs/*', r'import "', f.read())
    with open(os.path.join(pkg_dir, proto), 'w') as f:
        f.write(output)

protoc_command = './protoc/bin/protoc -I./protoc/include --proto_path=is_msgs --python_out=is_msgs {}'.format(' '.join(protos))
subprocess.call(['bash', '-c', protoc_command])
 
artifacts = [f for f in os.listdir(pkg_dir) if f.endswith('pb2.py')]
for artifact in artifacts:
    with open(os.path.join(pkg_dir, artifact), 'r') as f:
        output = re.sub(r'([^ ]import) ([a-zA-Z]+[\w]*_pb2[ as])', r'\1 is_msgs.\2', f.read())
    with open(os.path.join(pkg_dir, artifact), 'w') as f:
        f.write(output)

with open(os.path.join(pkg_dir, 'utils.py'), 'w') as f:
    f.write("""\
import os
import is_msgs

def get_include():
    return os.path.dirname(is_msgs.__file__)
""")

with open(os.path.join(pkg_dir, '__init__.py'), 'w') as f:
    f.write("""\
from utils import *
""")