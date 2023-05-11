#[[
# @brief	Guess the path to the Binary Module Interface (.ifc) from a Module Interface file (.ixx / .cppm)
#
# @argument target_name					Name of the target with C++20 code
# @argument module_interface_file       Name of the input Module Interface Source file (.ixx / .cppm)
# @argument out_bmi_file				The guessed output path for the Binary Module Interface (.ifc)
#
# @note     This function branches based on ${CMAKE_GENERATOR}
#]]
function(guess_bmi_path target_name module_interface_file out_bmi_file)

    # Gather target properties
    get_target_property(_TARGET__BINARY_DIR ${target_name} BINARY_DIR)
    get_target_property(_TARGET__NAME ${target_name} NAME)
    cmake_path(GET module_interface_file FILENAME _MI_FILE__FILENAME)

    # Guess the bmi path based on default build system behaviours
    if(CMAKE_GENERATOR STREQUAL "Visual Studio 17 2022")
        # Visual studio seems to set it's intermediate directory to ${_TARGET__NAME}.dir/$<CONFIG>/
        set(${out_bmi_file} "${_TARGET__BINARY_DIR}/${_TARGET__NAME}.dir/$<CONFIG>/${_MI_FILE__FILENAME}.ifc" PARENT_SCOPE)
    elseif(CMAKE_GENERATOR STREQUAL "Ninja")
        # Ninja seems to put them all in one big folder
        set(${out_bmi_file} "${_TARGET__BINARY_DIR}/${_MI_FILE__FILENAME}.ifc" PARENT_SCOPE) # TODO: Test, I'm not sure this is correct.
    endif()

endfunction()