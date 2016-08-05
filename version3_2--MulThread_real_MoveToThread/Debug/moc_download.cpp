/****************************************************************************
** Meta object code from reading C++ file 'download.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../download.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'download.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Download_t {
    QByteArrayData data[11];
    char stringdata0[121];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Download_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Download_t qt_meta_stringdata_Download = {
    {
QT_MOC_LITERAL(0, 0, 8), // "Download"
QT_MOC_LITERAL(1, 9, 16), // "DownloadFinished"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 15), // "Finished_Thread"
QT_MOC_LITERAL(4, 43, 12), // "httpFinished"
QT_MOC_LITERAL(5, 56, 13), // "httpReadyRead"
QT_MOC_LITERAL(6, 70, 11), // "updateSpeed"
QT_MOC_LITERAL(7, 82, 18), // "sendSpeed_leftSize"
QT_MOC_LITERAL(8, 101, 12), // "pair_2int64&"
QT_MOC_LITERAL(9, 114, 4), // "pair"
QT_MOC_LITERAL(10, 119, 1) // "i"

    },
    "Download\0DownloadFinished\0\0Finished_Thread\0"
    "httpFinished\0httpReadyRead\0updateSpeed\0"
    "sendSpeed_leftSize\0pair_2int64&\0pair\0"
    "i"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Download[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   44,    2, 0x06 /* Public */,
       3,    0,   45,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    0,   46,    2, 0x08 /* Private */,
       5,    0,   47,    2, 0x08 /* Private */,
       6,    0,   48,    2, 0x0a /* Public */,
       7,    2,   49,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 8, QMetaType::Int,    9,   10,

       0        // eod
};

void Download::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Download *_t = static_cast<Download *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->DownloadFinished(); break;
        case 1: _t->Finished_Thread(); break;
        case 2: _t->httpFinished(); break;
        case 3: _t->httpReadyRead(); break;
        case 4: _t->updateSpeed(); break;
        case 5: _t->sendSpeed_leftSize((*reinterpret_cast< pair_2int64(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (Download::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Download::DownloadFinished)) {
                *result = 0;
            }
        }
        {
            typedef void (Download::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Download::Finished_Thread)) {
                *result = 1;
            }
        }
    }
}

const QMetaObject Download::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Download.data,
      qt_meta_data_Download,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Download::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Download::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Download.stringdata0))
        return static_cast<void*>(const_cast< Download*>(this));
    return QObject::qt_metacast(_clname);
}

int Download::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
void Download::DownloadFinished()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}

// SIGNAL 1
void Download::Finished_Thread()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
