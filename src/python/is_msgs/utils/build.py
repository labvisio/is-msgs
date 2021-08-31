from __future__ import print_function
import os
import re
import sys
import shutil
import subprocess
import is_msgs

def get_include():
    return os.path.split(os.path.dirname(is_msgs.__file__))[0]

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print('Nothing to do. No *.proto files passed on arguments. Exiting...')
        sys.exit(-1)    
    for arg in sys.argv[1:]:
        if not arg.endswith('.proto'):
            print('\'{}\' isn\'t a *.proto file. Exiting...'.format(arg))
            sys.exit(-1)
        if not os.path.exists(arg):
            print('\'{}\' file doesn\'t exist. Exiting...'.format(arg))
            sys.exit(-1)
    
    if os.path.exists('tmp/'):
        shutil.rmtree('tmp/', ignore_errors=True)
    os.makedirs('tmp/')
    
    for proto in sys.argv[1:]:
        shutil.copy(proto, 'tmp/')
        with open(os.path.join('tmp/', proto), 'r') as f:
            output = re.sub(r'import "is/msgs/*', r'import "is_msgs/', f.read())
        with open(os.path.join('tmp/', proto), 'w') as f:
            f.write(output)
    
    protos = ' '.join([os.path.join('tmp/', proto) for proto in sys.argv[1:]])
    protoc_command='protoc -I=tmp/ -I={} --python_out=. {}'.format(get_include(), protos)
    subprocess.call(['bash', '-c', protoc_command])
    shutil.rmtree('tmp/', ignore_errors=True)
