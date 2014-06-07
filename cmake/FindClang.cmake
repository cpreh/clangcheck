include(
	FindPackageHandleStandardArgs
)

set(
	LLVM_CONFIG_COMMAND
	"llvm-config"
	CACHE
	STRING
	""
)

function(
	llvm_config
	OUTPUT
	FLAG
)
	execute_process(
		COMMAND ${LLVM_CONFIG_COMMAND}
		${FLAG}
		OUTPUT_VARIABLE
		TEMP_OUTPUT
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)

	set(
		${OUTPUT}
		${TEMP_OUTPUT}
		PARENT_SCOPE
	)
endfunction()

llvm_config(
	CLANG_CXXFLAGS
	--cppflags
)

llvm_config(
	CLANG_LDFLAGS
	--ldflags
)

set(
	CLANG_INCLUDE_DIRS
	${CLANG_INCLUDE_DIR}
)

find_package_handle_standard_args(
	Clang
	DEFAULT_MSG
	CLANG_CXXFLAGS
	CLANG_LDFLAGS
	CLANG_INCLUDE_DIRS
)
