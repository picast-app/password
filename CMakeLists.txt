cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 17)

project(passwords LANGUAGES CXX)

add_definitions(-DPI_SECRET="${SECRET}" -DAUTH_TOKEN="${AUTH}" -DPI_ID_SECRET="${ID_SECRET}")

find_package(aws-lambda-runtime REQUIRED)
find_package(AWSSDK COMPONENTS dynamodb)

include_directories(argon2/include)
link_directories(argon2)

add_executable(${PROJECT_NAME} 
  src/main.cpp
  src/handler.cpp
  src/hash.cpp
  src/salt.cpp
  src/db.cpp
  src/utils/json.cpp
  argon2/src/encoding.c
  argon2/src/blake2/blake2b.c
)
target_link_libraries(${PROJECT_NAME} PUBLIC argon2 AWS::aws-lambda-runtime ${AWSSDK_LINK_LIBRARIES})
aws_lambda_package_target(${PROJECT_NAME})
