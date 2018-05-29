from setuptools import setup

setup(
    name='is_msgs',
    version='1.1.2',
    description='Package with standard IS messages',
    url='http://github.com/labviros/is-msgs',
    author='labviros',
    license='MIT',
    packages=['is_msgs'],
    zip_safe=False,
    install_requires=[ 'protobuf' ]
)