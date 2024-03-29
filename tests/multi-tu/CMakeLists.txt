add_executable( multi-tu main.cc provider.cc )

target_link_libraries( multi-tu PRIVATE LLDEnzymeFlags )
set_target_properties(
    multi-tu PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}
)
