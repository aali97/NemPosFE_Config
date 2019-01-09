include(../../NemPosFE_Config.pri)

TEMPLATE = subdirs
CONFIG = ordered
SUBDIRS = \
        taopq

for(l, SUBDIRS) {
    LIBRARY_DEPENDS =
    include($$l/$${l}_dependencies.pri)
    lv = $${l}.depends
    $$lv = $$LIBRARY_DEPENDS
}
