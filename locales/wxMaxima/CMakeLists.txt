find_package(Gettext)

# which languages are supported?
file(GLOB TRANSLATION_FILES *.po)

# which sourcecode and data files can contain translatable text?
file(GLOB POT_SOURCE_FILES ${CMAKE_SOURCE_DIR}/src/*.cpp ${CMAKE_SOURCE_DIR}/src/*.h)

if(GETTEXT_FOUND)
  find_program(XGETTEXT xgettext)
  # generate a wxMaxima.pot in the build dir - don't touch the existing source (for now)
  # if that is okay, one may copy it to the source dir.
  # Currently the full source paths are used in the comments.
  message(STATUS "Create POT-file from current sourcecode")
  execute_process(
    COMMAND ${XGETTEXT} -C -k_ -s -o "wxMaxima.pot" ${POT_SOURCE_FILES})

  foreach(POFILE ${TRANSLATION_FILES})
    string(REGEX REPLACE ".*locales/wxMaxima/(.*).po$" "\\1" LANG ${POFILE})

    # convert the po to mo files and install them
    gettext_process_po_files(${LANG} ALL PO_FILES ${POFILE})
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${LANG}.gmo"
      DESTINATION "share/locale/${LANG}/LC_MESSAGES/"
      RENAME "wxMaxima.mo")

    # 'install' message files in the directory "${CMAKE_BINARY_DIR}/locale/..."
    # too (at build time, not at install time), so that running ./wxmaxima-local
    # (without installing it) can find the message files and i18n works.
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/locale/${LANG}")
    add_custom_target(build-locale-${LANG} ALL
      DEPENDS pofiles
      COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${LANG}.gmo" "${CMAKE_BINARY_DIR}/locale/${LANG}/LC_MESSAGES/wxMaxima.mo")
    # Call msgmerge for each language and create a merged language file
    message(STATUS "Create PO-file for language ${LANG}")
    execute_process(
      COMMAND ${GETTEXT_MSGMERGE_EXECUTABLE} -N ${CMAKE_SOURCE_DIR}/locales/wxMaxima/${LANG}.po ${CMAKE_BINARY_DIR}/wxMaxima.pot
      OUTPUT_FILE "${CMAKE_CURRENT_BINARY_DIR}/${LANG}.po.new"
      ERROR_QUIET)
    file(RENAME "${CMAKE_CURRENT_BINARY_DIR}/${LANG}.po.new" "${CMAKE_CURRENT_BINARY_DIR}/${LANG}.po")
  endforeach()

  message(STATUS "The new POT and PO files were generated in the build directory.")
  message(STATUS "Call 'make update-locale' to modify the wxMaxima sourcecode.")
  file(GLOB NEW_PO_FILES "${CMAKE_BINARY_DIR}/locales/*.po")

  add_custom_target(update-locale
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_BINARY_DIR}/wxMaxima.pot" "${CMAKE_SOURCE_DIR}/locales/wxMaxima/wxMaxima.pot"
    COMMAND ${CMAKE_COMMAND} -E copy "*.po" "${CMAKE_SOURCE_DIR}/locales/wxMaxima/"
    COMMENT "Installing wxMaxima.pot and po-files in the source directory ${CMAKE_SOURCE_DIR}/locales/wxMaxima")
else()
  message(STATUS "Gettext not found - translations of wxMaxima will be disabled")
endif()