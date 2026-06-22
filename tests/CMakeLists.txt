add_executable(tests
  main.cpp
  game/test_pet_battle.cpp
)

target_link_libraries(tests
  PRIVATE
    game
    Catch2::Catch2)

target_include_directories(tests
  PRIVATE
    ${CMAKE_BINARY_DIR})

catch_discover_tests(tests)

set_target_properties(tests
  PROPERTIES
    FOLDER "tests")

if(SCRIPTS)
  add_executable(tests-scripts
    main.cpp
    game/test_pet_battle.cpp
    scripts/test_tillers.cpp
  )

  target_link_libraries(tests-scripts
    PRIVATE
      scripts
      Catch2::Catch2)

  target_include_directories(tests-scripts
    PRIVATE
      ${CMAKE_BINARY_DIR})

  catch_discover_tests(tests-scripts)

  set_target_properties(tests-scripts
    PROPERTIES
      FOLDER "tests")
endif()
