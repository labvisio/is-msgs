import subprocess


def main():
    options_shared = [True, False]
    options_build_type = ["Debug", "Release"]
    for shared in options_shared:
        for build_type in options_build_type:
            command = f"""
                conan create . --user is --channel stable \
                    -s compiler=gcc \
                    -s compiler.version=11 \
                    -s compiler.libcxx=libstdc++11 \
                    -s build_type={build_type} \
                    -b missing \
                    -o:h is-msgs/*:shared={shared} \
                    -o:h is-msgs/*:build_tests=False
            """
            subprocess.call(['bash', '-c', command])


if __name__ == "__main__":
    main()
