option(LIBXMP_DOCS "Build the library documentation" ON)

set(libxmp_man_rst ${CMAKE_CURRENT_SOURCE_DIR}/manpage.rst)
set(libxmp_html_rst ${CMAKE_CURRENT_SOURCE_DIR}/pdfdoc.rst)
set(libxmp_man ${CMAKE_CURRENT_BINARY_DIR}/libxmp.3)
set(libxmp_html ${CMAKE_CURRENT_BINARY_DIR}/libxmp.html)
set(libxmp_pdf ${CMAKE_CURRENT_BINARY_DIR}/libxmp.pdf)

if(LIBXMP_DOCS)
  find_program(RST2MAN_EXE rst2man)
  find_program(RST2HTML_EXE rst2html)
  find_program(RST2PDF_EXE rst2pdf)

  if(NOT RST2MAN_EXE)
    message(STATUS "Manpage generation disabled: rst2man not found")
  else()
    message(STATUS "Found ${RST2MAN_EXE}")
    add_custom_command(OUTPUT ${libxmp_man}
      COMMAND ${RST2MAN_EXE} ${libxmp_man_rst} ${libxmp_man}
      DEPENDS ${libxmp_rst}
      COMMENT "Creating Info file ${libxmp_man}"
      VERBATIM)
    add_custom_target(libxmp_gen_man ALL DEPENDS ${libxmp_man})
    if(UNIX OR MINGW OR CYGWIN)
      include(GNUInstallDirs)
      install(FILES "${libxmp_man}"
              DESTINATION ${CMAKE_INSTALL_MANDIR}/man3
      )
    endif()
  endif()

  if(NOT RST2HTML_EXE)
    message(STATUS "HTML doc generation disabled: rst2html not found")
  else()
    message(STATUS "Found ${RST2HTML_EXE}")
    add_custom_command(OUTPUT ${libxmp_html}
      COMMAND ${RST2HTML_EXE} --stylesheet ${CMAKE_CURRENT_SOURCE_DIR}/style.css ${libxmp_html_rst} ${libxmp_html}
      DEPENDS ${libxmp_html_rst}
      COMMENT "Creating HTML file ${libxmp_html}"
      VERBATIM)
    add_custom_target(libxmp_gen_html ALL DEPENDS ${libxmp_html})
  endif()

  if(NOT RST2PDF_EXE)
    message(STATUS "PDF doc generation disabled: rst2pdf not found")
  else()
    message(STATUS "Found ${RST2PDF_EXE}")
    add_custom_command(OUTPUT ${libxmp_pdf}
      COMMAND ${RST2PDF_EXE} -c --smart-quotes=1 -s ${CMAKE_CURRENT_SOURCE_DIR}/custom.style --footer=\#\#\#Page\#\#\# ${libxmp_html_rst} -o ${libxmp_pdf}
      DEPENDS ${libxmp_html_rst}
      COMMENT "Creating HTML file ${libxmp_pdf}"
      VERBATIM)
    add_custom_target(libxmp_gen_pdf ALL DEPENDS ${libxmp_pdf})
  endif()
endif()
