include(FindPythonInterp) # ${PYTHON_EXECUTABLE}

set(GCS_BUCKET gs://www.boolexpr.org)

find_program(coverage_path coverage)
find_program(gsutil_path gsutil)
find_program(pylint_path pylint)

configure_file(boolexpr_build.py.in boolexpr_build.py)
configure_file(setup.py.in setup.py)
configure_file(boolexpr/__init__.py.in boolexpr/__init__.py)

configure_file(.pylintrc .pylintrc)
configure_file(test_boolexpr.py test_boolexpr.py)
configure_file(boolexpr/misc.py boolexpr/misc.py)
configure_file(boolexpr/util.py boolexpr/util.py)
configure_file(boolexpr/wrap.py boolexpr/wrap.py)

add_custom_command(
    OUTPUT ${OUTPUT}/stamp
    COMMAND ${PYTHON_EXECUTABLE} setup.py build_ext -i
)

if(coverage_path)
    add_custom_target(
        pycov
        COMMAND ${coverage_path} run test_boolexpr.py
        COMMAND ${coverage_path} html
        DEPENDS ${OUTPUT}/stamp
    )
endif()

add_custom_target(
    pytest
    COMMAND ${PYTHON_EXECUTABLE} setup.py test
)

if(pylint_path)
    add_custom_target(
        pylint
        COMMAND ${PYTHON_EXECUTABLE} setup.py build_ext -i
        COMMAND ${pylint_path} boolexpr
        DEPENDS ${OUTPUT}/stamp
    )
endif()

add_custom_target(
    html
    COMMAND ${PYTHON_EXECUTABLE} setup.py build_sphinx -s ${PROJECT_SOURCE_DIR}/doc/source
    DEPENDS ccapiref ${OUTPUT}/stamp
)

if(gsutil_path)
    add_custom_target(
        html-upload
        COMMAND ${gsutil_path} -m rsync -d -p -r python/build/sphinx/html ${GCS_BUCKET}
        COMMAND ${gsutil_path} -m acl ch -u AllUsers:R ${GCS_BUCKET}/**
        DEPENDS html
    )
endif()

add_custom_target(
    pypi-upload
    COMMAND ${PYTHON_EXECUTABLE} setup.py sdist --formats=gztar,zip
)
