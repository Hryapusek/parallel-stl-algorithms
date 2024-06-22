function(install_gtest)    
    include(FetchContent)
        FetchContent_Declare(
            googletest
            GIT https://github.com/google/googletest.git
        )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endfunction(install_gtest)
