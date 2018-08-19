include(FindPythonInterp) # ${PYTHON_EXECUTABLE}

set(GCS_BUCKET gs://www.boolexpr.org)

find_program(coverage_path coverage)
find_program(gsutil_path gsutil)
find_program(pylint_path pylint)

file(GLOB srcs "boolexpr/*.py")

file(COPY ../LICENSE ../README.md DESTINATION .)
file(COPY .pylintrc MANIFEST.in test_boolexpr.py DESTINATION .)
file(COPY ${srcs} DESTINATION boolexpr)
file(COPY ../include DESTINATION boolexpr/)
file(COPY ../src DESTINATION boolexpr/)
file(COPY ../third_party/glucosamine/src DESTINATION boolexpr/third_party/glucosamine/)
file(COPY ../third_party/boost-${BOOST_VERSION} DESTINATION boolexpr/third_party/)

configure_file(setup.py.in setup.py)
configure_file(boolexpr/__init__.py.in boolexpr/__init__.py)

add_custom_command(
    OUTPUT stamp
    COMMAND ${PYTHON_EXECUTABLE} setup.py build_ext -i
)

if (coverage_path)
    # Run unit tests with coverage
    add_custom_target(
        pycov
        COMMAND ${coverage_path} run test_boolexpr.py
        COMMAND ${coverage_path} html
        DEPENDS stamp
    )
endif (coverage_path)

if (DOXYGEN_FOUND)
    # Build Sphinx documentation
    add_custom_target(
        pyhtml
        COMMAND ${PYTHON_EXECUTABLE} setup.py build_sphinx
                -s ${PROJECT_SOURCE_DIR}/doc/source
        DEPENDS ccapiref stamp
    )
    # Upload to GCS
    if (gsutil_path)
        add_custom_target(
            pyhtml-upload
            COMMAND ${gsutil_path} -m rsync -d -p -r build/sphinx/html ${GCS_BUCKET}
            COMMAND ${gsutil_path} -m acl ch -u AllUsers:R ${GCS_BUCKET}/**
            DEPENDS pyhtml
        )
    endif (gsutil_path)
endif (DOXYGEN_FOUND)

if (pylint_path)
    # Run PyLint
    add_custom_target(
        pylint
        COMMAND ${PYTHON_EXECUTABLE} setup.py build_ext -i
        COMMAND ${pylint_path} boolexpr
        DEPENDS stamp
    )
endif (pylint_path)

add_custom_target(
    pytest
    COMMAND ${PYTHON_EXECUTABLE} setup.py test
)

add_custom_target(
    pypi-upload
    COMMAND ${PYTHON_EXECUTABLE} setup.py sdist --formats=gztar,zip
)
