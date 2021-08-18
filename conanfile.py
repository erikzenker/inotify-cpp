from conans import ConanFile, CMake, tools

class INotifyCpp(ConanFile):
    name = "inotifycpp"
    version = "1.0.0"
    author = "Erik Zenker"
    url = "https://github.com/erikzenker/inotify-cpp.git"
    description = "Inotify-cpp is a C++ wrapper for linux inotify"
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False]}
    default_options = {"shared": True}
    generators = "cmake"

    build_requires = "boost/[~1.76]"

    scm = {
        "type": "git",
        "subfolder": name,
        "url": "auto",
        "revision": "auto",
        "username": "git"
    }

    def _configure(self, verbose = True):
        cmake = CMake(self)
        cmake.verbose = verbose
        cmake.definitions['CMAKE_BUILD_TYPE'] = "Debug" if self.settings.build_type == "Debug" else "Release"
        cmake.definitions['BUILD_SHARED_LIBS'] = self.options.shared
        cmake.definitions['BUILD_STATIC_LIBS'] = not self.options.shared
        cmake.configure(source_folder = self.name)
        return cmake

    def build(self):
        cmake = self._configure()
        cmake.build()
        cmake.test(output_on_failure = True)

    def package(self):
        cmake = self._configure()
        cmake.install()
