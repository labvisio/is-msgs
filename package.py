from conan.packager import ConanMultiPackager

if __name__ == "__main__":
    builder = ConanMultiPackager(build_policy="missing")

    builder.add({
        "compiler.libcxx": "libstdc++11",
        "build_type": "Release"
    }, {"is-msgs:shared": True})

    builder.add({
        "compiler.libcxx": "libstdc++11",
        "build_type": "Release"
    }, {"is-msgs:shared": False})

    builder.run()