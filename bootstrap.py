import os
import shutil
import subprocess
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
if not os.path.exists(protoc_zip_file):
    protoc_link = 'https://github.com/google/protobuf/releases/download/v{}/{}'.format(protoc_version, protoc_zip_file)
    subprocess.call(['bash', '-c', 'wget {}'.format(protoc_link)])
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
    sed_command = 'sed -i \'s/import \"is\/msgs\//import \"/g\' {}'.format(os.path.join(pkg_dir, proto))
    subprocess.call(['bash', '-c', sed_command])

protoc_command = './protoc/bin/protoc -I./protoc/include --proto_path=is_msgs --python_out=is_msgs {}'.format(' '.join(protos))
subprocess.call(['bash', '-c', protoc_command])

os.mknod(os.path.join(pkg_dir, '__init__.py'))