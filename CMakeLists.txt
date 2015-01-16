cmake_minimum_required(
	VERSION
	2.8.12
)

project(
	clangcheck
	CXX
)

set(
	CMAKE_MODULE_PATH
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}"
)

find_package(
	Clang
	REQUIRED
)

add_library(
	clangcheck
	SHARED
	main.cpp
)

# TODO: How do we set this per target?
# TODO: What do we really need here?
add_definitions(
	${CLANG_CXXFLAGS}
	-fno-common
	-ffunction-sections -fdata-sections
	-fvisibility-inlines-hidden
	-fno-exceptions -fno-rtti
	-fno-strict-aliasing
)

target_include_directories(
	clangcheck
	PRIVATE
	${CLANG_INCLUDE_DIRS}
)

# TODO: What do we really need here?
set_target_properties(
	clangcheck
	PROPERTIES
	LINK_FLAGS
	"-Wl,--no-undefined
	${CLANG_LDFLAGS}
	-Wl,--gc-sections
	-Wl,--version-script=${CMAKE_CURRENT_SOURCE_DIR}/ExportedSymbolsList"
)

target_compile_options(
	clangcheck
	PRIVATE
	-std=c++11 -Wall
)

# TODO: What do we really need here?
target_link_libraries(
	clangcheck
	PRIVATE
	clang
	clangAnalysis
	clangARCMigrate
	clangAST
	clangASTMatchers
	clangBasic
	clangCodeGen
	clangDriver
	clangDynamicASTMatchers
	clangEdit
	clangFormat
	clangFrontend
	clangFrontendTool
	clangIndex
	clangLex
	clangParse
	clangRewriteFrontend
	clangSema
	clangSerialization
	clangStaticAnalyzerCheckers
	clangStaticAnalyzerCore
	clangStaticAnalyzerFrontend
	clangTooling
	LLVMAnalysis
	LLVMAsmParser
	LLVMAsmPrinter
	LLVMBitReader
	LLVMBitWriter
	LLVMCodeGen
	LLVMCore
	LLVMDebugInfo
	LLVMExecutionEngine
	LLVMInstCombine
	LLVMInstrumentation
	LLVMInterpreter
	LLVMipa
	LLVMipo
	LLVMIRReader
	LLVMLineEditor
	LLVMLinker
	LLVMLTO
	LLVMMC
	LLVMMCDisassembler
	LLVMMCJIT
	LLVMMCParser
	LLVMObjCARCOpts
	LLVMObject
	LLVMOption
	LLVMProfileData
	LLVMRuntimeDyld
	LLVMScalarOpts
	LLVMSelectionDAG
	LLVMSupport
	LLVMTableGen
	LLVMTarget
	LLVMTransformUtils
	LLVMVectorize
	LLVMX86AsmParser
	LLVMX86AsmPrinter
	LLVMX86CodeGen
	LLVMX86Desc
	LLVMX86Disassembler
	LLVMX86Info
	LLVMX86Utils
	dl
	ncurses
	pthread
)
