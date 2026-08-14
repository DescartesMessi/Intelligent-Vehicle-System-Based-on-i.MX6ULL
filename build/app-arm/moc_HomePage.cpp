/****************************************************************************
** Meta object code from reading C++ file 'HomePage.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.9)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../app/pages/home/HomePage.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'HomePage.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.9. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_HomePage_t {
    QByteArrayData data[8];
    char stringdata0[110];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_HomePage_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_HomePage_t qt_meta_stringdata_HomePage = {
    {
QT_MOC_LITERAL(0, 0, 8), // "HomePage"
QT_MOC_LITERAL(1, 9, 14), // "audioRequested"
QT_MOC_LITERAL(2, 24, 0), // ""
QT_MOC_LITERAL(3, 25, 14), // "videoRequested"
QT_MOC_LITERAL(4, 40, 16), // "reverseRequested"
QT_MOC_LITERAL(5, 57, 17), // "sentinelRequested"
QT_MOC_LITERAL(6, 75, 19), // "diagnosticRequested"
QT_MOC_LITERAL(7, 95, 14) // "updateDateTime"

    },
    "HomePage\0audioRequested\0\0videoRequested\0"
    "reverseRequested\0sentinelRequested\0"
    "diagnosticRequested\0updateDateTime"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_HomePage[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   44,    2, 0x06 /* Public */,
       3,    0,   45,    2, 0x06 /* Public */,
       4,    0,   46,    2, 0x06 /* Public */,
       5,    0,   47,    2, 0x06 /* Public */,
       6,    0,   48,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    0,   49,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void HomePage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<HomePage *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->audioRequested(); break;
        case 1: _t->videoRequested(); break;
        case 2: _t->reverseRequested(); break;
        case 3: _t->sentinelRequested(); break;
        case 4: _t->diagnosticRequested(); break;
        case 5: _t->updateDateTime(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (HomePage::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&HomePage::audioRequested)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (HomePage::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&HomePage::videoRequested)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (HomePage::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&HomePage::reverseRequested)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (HomePage::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&HomePage::sentinelRequested)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (HomePage::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&HomePage::diagnosticRequested)) {
                *result = 4;
                return;
            }
        }
    }
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject HomePage::staticMetaObject = { {
    &QWidget::staticMetaObject,
    qt_meta_stringdata_HomePage.data,
    qt_meta_data_HomePage,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *HomePage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *HomePage::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_HomePage.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int HomePage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void HomePage::audioRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void HomePage::videoRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void HomePage::reverseRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void HomePage::sentinelRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void HomePage::diagnosticRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
