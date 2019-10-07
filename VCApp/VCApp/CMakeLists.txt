cmake_minimum_required(VERSION 3.8)

find_package(SFML 2.5 REQUIRED COMPONENTS graphics window system )
find_package( SFGUI 0.4.0 REQUIRED )

FILE(GLOB VCApp_Src *.cpp *.c)
FILE(GLOB VCApp_h *.h)


add_executable(VCApp ${VCApp_Src} ${VCApp_h} )
target_link_libraries(VCApp BoostQVM NemaTode MathLib3D sfml-system sfml-network  ${SFGUI_LIBRARY}  ${SFGUI_DEPENDENCIES})

FixExePostfix(VCApp)

FixStaticLinking()
