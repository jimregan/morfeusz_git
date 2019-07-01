include(UseJava)
find_package(Java REQUIRED)

# include_directories (${JAVA_INCLUDE_PATH})
# include_directories (${JAVA_INCLUDE_PATH2})
# include_directories (${CMAKE_SOURCE_DIR}/morfeusz)

set(GUI_SOURCES
  morfeusz/AboutDialog.java
  morfeusz/Agent.java
  morfeusz/DictSearchPathsDialog.java
  morfeusz/Morfeusz.java
  morfeusz/OptionsDialog.java
  morfeusz/PlaceHolder.java
  morfeusz/Preferences.java
  morfeusz/RecentFileAction.java
  morfeusz/ResultsPane.java
  morfeusz/SelectFileDialog.java
  morfeusz/ToolBarButton.java
  )

# set(GUI_IMAGES
#   images/book_open_disabled.png images/book_open.png
#   images/book_open_pressed.png images/book_open_roll_over.png
#   images/conf.png images/conf_pressed.png images/conf_roll_over.png
#   images/down.gif images/end_node.gif images/font_decrease.png
#   images/font_decrease_pressed.png images/font_decrease_roll_over.png
#   images/font_increase.png images/font_increase_pressed.png
#   images/font_increase_roll_over.png images/info.png
#   images/info_pressed.png images/info_roll_over.png images/open.png
#   images/open_pressed.png images/open_roll_over.png images/return.png
#   images/return_pressed.png images/return_roll_over.png
#   images/save_disabled.png images/save.png images/save_pressed.png
#   images/save_roll_over.png images/start_node.gif
#   )

# kopiujemy, żeby zip poniżej mógł je dodać bez ścieżki:
file(GLOB IMAGE_FILES "images/*")
file(COPY ${IMAGE_FILES} DESTINATION .)
file(COPY quitaboutpreferenceshandler/ DESTINATION quitaboutpreferenceshandler/)

# to jest w zasadzie niepotrzebne, chyba że chcemy odpalić skompilowanego morfeusz.jar bez instalacji:
#file(COPY QuitAboutPreferencesHandler.jar DESTINATION .)

add_jar (gui-proto-jar
  SOURCES "${GUI_SOURCES}"
  #"${GUI_IMAGES}"
#  INCLUDE_JARS jmorfeusz.jar QuitAboutPreferencesHandler.jar images.jar
#  INCLUDE_JARS ${CMAKE_CURRENT_BINARY_DIR}/../morfeusz/wrappers/java/jmorfeusz.jar QuitAboutPreferencesHandler.jar Images/*.png
  INCLUDE_JARS jmorfeusz  QuitAboutPreferencesHandler.jar
  ENTRY_POINT morfeusz/Morfeusz
# Moim zdaniem byłoby eleganciej z Manifestem włączającym systemowy
#  jmorfeusz.jar, ale 14.04 (cmake 2.8.12) nie rozumie tego, więc
#  zamiast wzipowujemy całe jmorfeusz poniżej do gui-jar:
#
# MANIFEST  MANIFEST.MF
  OUTPUT_NAME morfeusz
  )

add_dependencies(gui-proto-jar jmorfeusz)

add_custom_target(gui-jar
  COMMAND zip -q morfeusz.jar "*.png" "*.gif"
  COMMAND zip -q morfeusz.jar "quitaboutpreferenceshandler/*"
  COMMAND ln -s ../morfeusz/wrappers/java/pl .
  COMMAND zip -qr morfeusz.jar "pl"
  DEPENDS gui-proto-jar
  )


add_custom_target (gui-deb
  COMMAND "${PROJECT_SOURCE_DIR}/createGUIDeb.sh" "${PROJECT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}" "${Morfeusz_DEB_VERSION}" "${ARCHITECTURE}" "${TARGET_DIR}"
  DEPENDS gui-jar
  )

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  if (${ARCHITECTURE} MATCHES "amd64")
    file(COPY morfeusz-gui64.exe DESTINATION ./morfeusz-gui.exe)
  else ()
    file(COPY morfeusz-gui32.exe DESTINATION ./morfeusz-gui.exe)
  endif ()
  set (NSIS_SCRIPT "${PROJECT_SOURCE_DIR}/morfeusz.nsi")

  add_custom_target (gui-installer
    COMMAND makensis -NOCD "-DVERSION=${Morfeusz_LIB_VERSION}.${DICT_VERSION}" "$-DARCHITECTURE={ARCHITECTURE}" "-DTARGET_DIR=${TARGET_DIR}" "${NSIS_SCRIPT}"
    DEPENDS gui-jar "${NSIS_SCRIPT}"
    )
  
endif ()
    
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(MAC_APP_DIR "dmg/Morfeusz.app")
  file(COPY Morfeusz.app/ DESTINATION "${MAC_APP_DIR}")

  add_custom_target(gui-dmg
    COMMAND install -D "${CMAKE_CURRENT_BINARY_DIR}/../morfeusz/libmorfeusz2.dylib" "${MAC_APP_DIR}/Contents/MacOS/libmorfeusz2.dylib"
    COMMAND install -D "${CMAKE_CURRENT_BINARY_DIR}/../morfeusz/wrappers/java/libjmorfeusz.jnilib" "${MAC_APP_DIR}/Contents/MacOS/libjmorfeusz.jnilib"
    COMMAND install -D "${CMAKE_CURRENT_BINARY_DIR}/morfeusz.jar" "${MAC_APP_DIR}/Contents/Java/morfeusz.jar"
#    COMMAND install -D "${CMAKE_CURRENT_BINARY_DIR}/../morfeusz/wrappers/java/jmorfeusz.jar" "${MAC_APP_DIR}/Contents/Java/jmorfeusz.jar"
    COMMAND genisoimage -V Morfeusz2 -D -R -apple -no-pad -o "${TARGET_DIR}/morfeusz2-gui_${Morfeusz_LIB_VERSION}.${DICT_VERSION}-${CMAKE_SYSTEM_NAME}-${ARCHITECTURE}.dmg" "dmg/"
    DEPENDS gui-jar "${NSIS_SCRIPT}"
    )
endif ()
