add_library(NeatReflectionIfcProcessingUtilities 
	"include/ContextualException.h" "src/ContextualException.cpp" 
	"include/IfcRendering.h" "src/IfcRendering.cpp" 
	"include/IfcConversion.h" "src/IfcConversion.cpp" 
	"include/IfcVisitor.h" 
	"include/StringOperations.h" "src/StringOperations.cpp"
	"include/MioBlobHolder.h")
target_include_directories(NeatReflectionIfcProcessingUtilities PUBLIC "include")
target_compile_features(NeatReflectionIfcProcessingUtilities PUBLIC cxx_std_20)
target_link_libraries(NeatReflectionIfcProcessingUtilities PUBLIC NeatReflection reflifc mio)
set_target_properties(NeatReflectionIfcProcessingUtilities PROPERTIES FOLDER "Neat/Tools")
