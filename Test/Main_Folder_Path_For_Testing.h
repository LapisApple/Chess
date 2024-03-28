//
// Created by timap on 19.03.2024.
//

#pragma once
#include <string>

/*
 example:
 main_folder_path = "/home/LapisApple/UNI/s01/PK1/tim.apel/project"
 main_folder_path = "C:/Users/timap/Documents/UNI/UniGitlab/C++1/tim.apel/project";
*/
inline const std::string main_folder_path = "C:/Users/timap/Desktop/Chess";

// error message for missing folder path
inline const char* missing_main_folder_path_error =
    "Please change the folder path to this project in project/Test/Main_Folder_Path_For_Testing.h if you see this "
    "error";
