!isEmpty(EAS_PRI_INCLUDED):error("NemPosFE_Config.pri already included")
EAS_PRI_INCLUDED = 1

defineReplace(libraryName) {
    unset(LIBRARY_NAME)
    LIBRARY_NAME = $$1
    CONFIG(debug, debug|release) {
       !debug_and_release|build_pass {
           mac:RET = $$member(LIBRARY_NAME, 0)_debug
           else:win32:RET = $$member(LIBRARY_NAME, 0)d
       }
    }
    isEmpty(RET):RET = $$LIBRARY_NAME
    return($$RET)
}

IDE_SOURCE_TREE = $$PWD
IDE_BIN_PATH = $$IDE_SOURCE_TREE/bin
IDE_LIBS_PATH = $$IDE_BIN_PATH/libs
LIBS_DIR = $$IDE_SOURCE_TREE/src/libs
3RDPARTYLIBS_DIR = $$PWD/libs-3rdparty

!isEmpty(LIBRARY_DEPENDS) {
    LIBS *= -L$$IDE_LIBS_PATH
    LIBS *= -L$$IDE_BIN_PATH
}

done_libs =
for(ever) {
    isEmpty(LIBRARY_DEPENDS): \
        break()
    done_libs += $$LIBRARY_DEPENDS
    for(dep, LIBRARY_DEPENDS) {
        include($$PWD/src/libs/$$dep/$${dep}_dependencies.pri)
        LIBS *= -l$$libraryName($$LIBRARY_NAME)
        INCLUDEPATH *= $$PWD/src/libs/
        INCLUDEPATH *= $$PWD/src/libs/$$dep/
        INCLUDEPATH *= $$PWD/src/libs/$$dep/include/
        INCLUDEPATH *= $$PWD/src/libs/$$dep/shared/
    }
    LIBRARY_DEPENDS = $$unique(LIBRARY_DEPENDS)
    LIBRARY_DEPENDS -= $$unique(done_libs)
}
