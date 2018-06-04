from setuptools import setup

setup(
    name='is_msgs',
    version='1.1.2.1',
    description='Package with standard IS messages',
    url='http://github.com/labviros/is-msgs',
    author='labviros',
    license='MIT',
    packages=['is_msgs'],
    package_data={ 'is_msgs': ['*.proto'] },
    zip_safe=False,
    install_requires=[ 'protobuf' ]
)