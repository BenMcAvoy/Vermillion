#pragma once

namespace Vermilion {
	template<typename T>
	concept Addressable = std::is_pointer_v<T> ||
		(std::is_integral_v<T> && sizeof(T) == 8);

	using Address = uintptr_t;

	template <Addressable U>
	constexpr Address GetAddress(U value) {
		if constexpr (std::is_pointer_v<U>) {
			return reinterpret_cast<Address>(value);
		} else {
			return static_cast<Address>(value);
		}
	}

	class Driver_t {
	public:
		Driver_t() {
			hDriver_ = CreateFile(
				Vermilion::DriverPath,
				GENERIC_READ | GENERIC_WRITE,
				NULL,
				nullptr,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL,
				nullptr
			);

			if (hDriver_ == INVALID_HANDLE_VALUE) {
				throw std::runtime_error("Failed to open driver handle.");
			}

			targetPID_ = FindPID(Vermilion::ProcessName);

			Buffer initBuffer = { 0 };
			initBuffer.targetPID = targetPID_;
			IoCtlCall(INITCODE, initBuffer, "initialize driver for target process");
			IoCtlCall(GETSBADDR, initBuffer, "get target base address");

			baseAddress_ = static_cast<uintptr_t>(initBuffer.targetAddress);
		}

		~Driver_t() {
			if (hDriver_ != INVALID_HANDLE_VALUE) {
				CloseHandle(hDriver_);
			}
		}

		template <typename T, Addressable U>
		T Read(U address) {
			T value{};
			IoCtlReq(
				READCODE,
				GetAddress(address),
				&value,
				sizeof(T),
				"read memory"
			);
			return value;
		}

		template <Addressable U>
		void Read(U address, void* out, size_t size) {
			IoCtlReq(
				READCODE,
				GetAddress(address),
				out,
				size,
				"read memory"
			);
		}

		template <typename T, Addressable U>
		void Write(U address, T& value) {
			static_assert(std::is_trivially_copyable_v<T>, "T must be trivially copyable");
			IoCtlReq(
				WRITECODE,
				GetAddress(address),
				&value,
				sizeof(T),
				"write memory"
			);
		}

		uintptr_t GetBaseAddress() const {
			return baseAddress_;
		}

	private:
		Driver_t(const Driver_t&) = delete;
		Driver_t& operator=(const Driver_t&) = delete;

		void IoCtlReq(
			DWORD ioctl,
			Address target,
			void* buffer,
			size_t size,
			const char* actionContext
		) const {
			static Buffer ioBuffer = { 0 };
			memset(&ioBuffer, 0, sizeof(Buffer));

			ioBuffer.targetAddress	= target;
			ioBuffer.bufferAddress	= reinterpret_cast<UINT64>(buffer);
			ioBuffer.bytesToRead	= size;

			DWORD bytesReturned = 0;
			BOOL ok = DeviceIoControl(
				hDriver_,
				ioctl,
				&ioBuffer,
				sizeof(Buffer),
				&ioBuffer,
				sizeof(Buffer),
				&bytesReturned,
				NULL
			);

			if (!ok || bytesReturned == 0) {
				throw std::runtime_error(std::string("Failed to ") + actionContext + " via driver.");
			}

			if (ioBuffer.bytesRead != size) {
				throw std::runtime_error(std::string("Incomplete ") + actionContext + " via driver.");
			}
		}

		template <typename T>
		void IoCtlCall(
			DWORD ioctl,
			T& buffer,
			const char* action
		) const {
			if (!hDriver_) {
				throw std::runtime_error(
					"Driver not initialized"
				);
			}

			DWORD bytesReturned = 0;
			BOOL ok = DeviceIoControl(
				hDriver_,
				ioctl,
				&buffer,
				sizeof(T),
				&buffer,
				sizeof(T),
				&bytesReturned,
				nullptr
			);

			if (!ok) {
				DWORD err = GetLastError();
				throw std::runtime_error(
					std::format(
						"{} failed: error {} (0x{:X})",
						action, err, err
					)
				);
			}

			if (bytesReturned != sizeof(T)) {
				throw std::runtime_error(
					std::format(
						"Partial {} ioctl: expected {}, got {}",
						action, sizeof(T), bytesReturned
					)
				);
			}
		}

		static DWORD FindPID(const std::wstring& processName) {
			auto hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
			if (hSnap == INVALID_HANDLE_VALUE)
				throw std::runtime_error("Failed to create process snapshot.");

			PROCESSENTRY32 pe32{ sizeof(PROCESSENTRY32) };
			if (!Process32First(hSnap, &pe32)) {
				CloseHandle(hSnap);
				throw std::runtime_error("Failed to get first process.");
			}

			DWORD pid = 0;

			do {
				if (std::wstring(pe32.szExeFile) == processName) {
					pid = pe32.th32ProcessID;
					break;
				}
			} while (Process32Next(hSnap, &pe32));

			CloseHandle(hSnap);
			return pid;
		}

		HANDLE hDriver_ = INVALID_HANDLE_VALUE;
		DWORD targetPID_ = 0;
		uintptr_t baseAddress_ = 0;

		static inline constexpr ULONG INITCODE = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x775, METHOD_BUFFERED, FILE_SPECIAL_ACCESS); // attach
		static inline constexpr ULONG READCODE = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x776, METHOD_BUFFERED, FILE_SPECIAL_ACCESS); // read
		static inline constexpr ULONG WRITECODE = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x777, METHOD_BUFFERED, FILE_SPECIAL_ACCESS); // write
		static inline constexpr ULONG GETSBADDR = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x778, METHOD_BUFFERED, FILE_SPECIAL_ACCESS); // get base addr

		struct Buffer {
			_In_    UINT64 targetPID;
			_In_    UINT64 targetAddress;
			_Inout_ UINT64 bufferAddress;
			_In_    UINT64 bytesToRead;
			_Out_   UINT64 bytesRead;
		};
	};

	static inline Driver_t GDriver;
} // namespace Vermilion
