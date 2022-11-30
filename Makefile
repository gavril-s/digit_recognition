dpnds:
	git clone https://github.com/microsoft/vcpkg
	.\vcpkg\bootstrap-vcpkg.bat
	.\vcpkg\vcpkg.exe install drogon --host-triplet=x86-windows
	.\vcpkg\vcpkg.exe integrate install