/****************************************************************************
** Meta object code from reading C++ file 'RZEFrameGraphEditor.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "rzepch.h"
#include "../UI/Windows/RZEFrameGraphEditor.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'RZEFrameGraphEditor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Razix__Editor__FrameGraphGraphicsView_t {
    QByteArrayData data[6];
    char stringdata0[103];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Razix__Editor__FrameGraphGraphicsView_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Razix__Editor__FrameGraphGraphicsView_t qt_meta_stringdata_Razix__Editor__FrameGraphGraphicsView = {
    {
QT_MOC_LITERAL(0, 0, 37), // "Razix::Editor::FrameGraphGrap..."
QT_MOC_LITERAL(1, 38, 13), // "OnAddPassNode"
QT_MOC_LITERAL(2, 52, 0), // ""
QT_MOC_LITERAL(3, 53, 15), // "OnAddBufferNode"
QT_MOC_LITERAL(4, 69, 16), // "OnAddTextureNode"
QT_MOC_LITERAL(5, 86, 16) // "OnImportResource"

    },
    "Razix::Editor::FrameGraphGraphicsView\0"
    "OnAddPassNode\0\0OnAddBufferNode\0"
    "OnAddTextureNode\0OnImportResource"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Razix__Editor__FrameGraphGraphicsView[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   34,    2, 0x0a /* Public */,
       3,    0,   35,    2, 0x0a /* Public */,
       4,    0,   36,    2, 0x0a /* Public */,
       5,    0,   37,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Razix::Editor::FrameGraphGraphicsView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<FrameGraphGraphicsView *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->OnAddPassNode(); break;
        case 1: _t->OnAddBufferNode(); break;
        case 2: _t->OnAddTextureNode(); break;
        case 3: _t->OnImportResource(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject Razix::Editor::FrameGraphGraphicsView::staticMetaObject = { {
    QMetaObject::SuperData::link<NodeGraphicsView::staticMetaObject>(),
    qt_meta_stringdata_Razix__Editor__FrameGraphGraphicsView.data,
    qt_meta_data_Razix__Editor__FrameGraphGraphicsView,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Razix::Editor::FrameGraphGraphicsView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Razix::Editor::FrameGraphGraphicsView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Razix__Editor__FrameGraphGraphicsView.stringdata0))
        return static_cast<void*>(this);
    return NodeGraphicsView::qt_metacast(_clname);
}

int Razix::Editor::FrameGraphGraphicsView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = NodeGraphicsView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}
struct qt_meta_stringdata_Razix__Editor__RZEFrameGraphEditor_t {
    QByteArrayData data[42];
    char stringdata0[969];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Razix__Editor__RZEFrameGraphEditor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Razix__Editor__RZEFrameGraphEditor_t qt_meta_stringdata_Razix__Editor__RZEFrameGraphEditor = {
    {
QT_MOC_LITERAL(0, 0, 34), // "Razix::Editor::RZEFrameGraphE..."
QT_MOC_LITERAL(1, 35, 27), // "OnImportPresetButtonClicked"
QT_MOC_LITERAL(2, 63, 0), // ""
QT_MOC_LITERAL(3, 64, 14), // "OnNodeSelected"
QT_MOC_LITERAL(4, 79, 5), // "Node*"
QT_MOC_LITERAL(5, 85, 4), // "node"
QT_MOC_LITERAL(6, 90, 21), // "OnPassNodeNameChanged"
QT_MOC_LITERAL(7, 112, 20), // "OnAddInputPinClicked"
QT_MOC_LITERAL(8, 133, 21), // "OnInputPinNameChanged"
QT_MOC_LITERAL(9, 155, 3), // "idx"
QT_MOC_LITERAL(10, 159, 23), // "OnRemoveInputPinClicked"
QT_MOC_LITERAL(11, 183, 21), // "OnAddOutputPinClicked"
QT_MOC_LITERAL(12, 205, 22), // "OnOutputPinNameChanged"
QT_MOC_LITERAL(13, 228, 24), // "OnRemoveOutputPinClicked"
QT_MOC_LITERAL(14, 253, 23), // "OnAddColorFormatClicked"
QT_MOC_LITERAL(15, 277, 20), // "OnColorFormatChanged"
QT_MOC_LITERAL(16, 298, 26), // "OnRemoveColorFormatClicked"
QT_MOC_LITERAL(17, 325, 21), // "OnBrowseShaderPressed"
QT_MOC_LITERAL(18, 347, 21), // "OnPipelineNameChanged"
QT_MOC_LITERAL(19, 369, 26), // "OnPipelineCullModeSelected"
QT_MOC_LITERAL(20, 396, 29), // "OnPipelinePolygonModeSelected"
QT_MOC_LITERAL(21, 426, 26), // "OnPipelineDrawTypeSelected"
QT_MOC_LITERAL(22, 453, 35), // "OnPipelineEnableTransparencyC..."
QT_MOC_LITERAL(23, 489, 32), // "OnPipelineEnableDepthTestChecked"
QT_MOC_LITERAL(24, 522, 33), // "OnPipelineEnableDepthWriteChe..."
QT_MOC_LITERAL(25, 556, 32), // "OnPipelineDepthOperationSelected"
QT_MOC_LITERAL(26, 589, 26), // "OnPipelineColorSrcSelected"
QT_MOC_LITERAL(27, 616, 26), // "OnPipelineColorDstSelected"
QT_MOC_LITERAL(28, 643, 32), // "OnPipelineColorOperationSelected"
QT_MOC_LITERAL(29, 676, 26), // "OnPipelineAlphaSrcSelected"
QT_MOC_LITERAL(30, 703, 26), // "OnPipelineAlphaDstSelected"
QT_MOC_LITERAL(31, 730, 32), // "OnPipelineAlphaOperationSelected"
QT_MOC_LITERAL(32, 763, 29), // "OnPipelineDepthFormatSelected"
QT_MOC_LITERAL(33, 793, 27), // "OnSceneGeometryModeSelected"
QT_MOC_LITERAL(34, 821, 14), // "OnEnableResize"
QT_MOC_LITERAL(35, 836, 20), // "OnResolutionSelected"
QT_MOC_LITERAL(36, 857, 16), // "OnExtentXChanged"
QT_MOC_LITERAL(37, 874, 16), // "OnExtentYChanged"
QT_MOC_LITERAL(38, 891, 15), // "OnLayersChanged"
QT_MOC_LITERAL(39, 907, 19), // "OnBufferNameChanged"
QT_MOC_LITERAL(40, 927, 19), // "OnBufferSizeChanged"
QT_MOC_LITERAL(41, 947, 21) // "OnBufferUsageSelected"

    },
    "Razix::Editor::RZEFrameGraphEditor\0"
    "OnImportPresetButtonClicked\0\0"
    "OnNodeSelected\0Node*\0node\0"
    "OnPassNodeNameChanged\0OnAddInputPinClicked\0"
    "OnInputPinNameChanged\0idx\0"
    "OnRemoveInputPinClicked\0OnAddOutputPinClicked\0"
    "OnOutputPinNameChanged\0OnRemoveOutputPinClicked\0"
    "OnAddColorFormatClicked\0OnColorFormatChanged\0"
    "OnRemoveColorFormatClicked\0"
    "OnBrowseShaderPressed\0OnPipelineNameChanged\0"
    "OnPipelineCullModeSelected\0"
    "OnPipelinePolygonModeSelected\0"
    "OnPipelineDrawTypeSelected\0"
    "OnPipelineEnableTransparencyChecked\0"
    "OnPipelineEnableDepthTestChecked\0"
    "OnPipelineEnableDepthWriteChecked\0"
    "OnPipelineDepthOperationSelected\0"
    "OnPipelineColorSrcSelected\0"
    "OnPipelineColorDstSelected\0"
    "OnPipelineColorOperationSelected\0"
    "OnPipelineAlphaSrcSelected\0"
    "OnPipelineAlphaDstSelected\0"
    "OnPipelineAlphaOperationSelected\0"
    "OnPipelineDepthFormatSelected\0"
    "OnSceneGeometryModeSelected\0OnEnableResize\0"
    "OnResolutionSelected\0OnExtentXChanged\0"
    "OnExtentYChanged\0OnLayersChanged\0"
    "OnBufferNameChanged\0OnBufferSizeChanged\0"
    "OnBufferUsageSelected"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Razix__Editor__RZEFrameGraphEditor[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      37,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,  199,    2, 0x0a /* Public */,
       3,    1,  200,    2, 0x0a /* Public */,
       6,    0,  203,    2, 0x0a /* Public */,
       7,    0,  204,    2, 0x0a /* Public */,
       8,    1,  205,    2, 0x0a /* Public */,
      10,    0,  208,    2, 0x0a /* Public */,
      11,    0,  209,    2, 0x0a /* Public */,
      12,    1,  210,    2, 0x0a /* Public */,
      13,    0,  213,    2, 0x0a /* Public */,
      14,    0,  214,    2, 0x0a /* Public */,
      15,    1,  215,    2, 0x0a /* Public */,
      16,    0,  218,    2, 0x0a /* Public */,
      17,    0,  219,    2, 0x0a /* Public */,
      18,    0,  220,    2, 0x0a /* Public */,
      19,    0,  221,    2, 0x0a /* Public */,
      20,    0,  222,    2, 0x0a /* Public */,
      21,    0,  223,    2, 0x0a /* Public */,
      22,    0,  224,    2, 0x0a /* Public */,
      23,    0,  225,    2, 0x0a /* Public */,
      24,    0,  226,    2, 0x0a /* Public */,
      25,    0,  227,    2, 0x0a /* Public */,
      26,    0,  228,    2, 0x0a /* Public */,
      27,    0,  229,    2, 0x0a /* Public */,
      28,    0,  230,    2, 0x0a /* Public */,
      29,    0,  231,    2, 0x0a /* Public */,
      30,    0,  232,    2, 0x0a /* Public */,
      31,    0,  233,    2, 0x0a /* Public */,
      32,    0,  234,    2, 0x0a /* Public */,
      33,    0,  235,    2, 0x0a /* Public */,
      34,    0,  236,    2, 0x0a /* Public */,
      35,    0,  237,    2, 0x0a /* Public */,
      36,    0,  238,    2, 0x0a /* Public */,
      37,    0,  239,    2, 0x0a /* Public */,
      38,    0,  240,    2, 0x0a /* Public */,
      39,    0,  241,    2, 0x0a /* Public */,
      40,    0,  242,    2, 0x0a /* Public */,
      41,    0,  243,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4,    5,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    9,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    9,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    9,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Razix::Editor::RZEFrameGraphEditor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<RZEFrameGraphEditor *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->OnImportPresetButtonClicked(); break;
        case 1: _t->OnNodeSelected((*reinterpret_cast< Node*(*)>(_a[1]))); break;
        case 2: _t->OnPassNodeNameChanged(); break;
        case 3: _t->OnAddInputPinClicked(); break;
        case 4: _t->OnInputPinNameChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->OnRemoveInputPinClicked(); break;
        case 6: _t->OnAddOutputPinClicked(); break;
        case 7: _t->OnOutputPinNameChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->OnRemoveOutputPinClicked(); break;
        case 9: _t->OnAddColorFormatClicked(); break;
        case 10: _t->OnColorFormatChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: _t->OnRemoveColorFormatClicked(); break;
        case 12: _t->OnBrowseShaderPressed(); break;
        case 13: _t->OnPipelineNameChanged(); break;
        case 14: _t->OnPipelineCullModeSelected(); break;
        case 15: _t->OnPipelinePolygonModeSelected(); break;
        case 16: _t->OnPipelineDrawTypeSelected(); break;
        case 17: _t->OnPipelineEnableTransparencyChecked(); break;
        case 18: _t->OnPipelineEnableDepthTestChecked(); break;
        case 19: _t->OnPipelineEnableDepthWriteChecked(); break;
        case 20: _t->OnPipelineDepthOperationSelected(); break;
        case 21: _t->OnPipelineColorSrcSelected(); break;
        case 22: _t->OnPipelineColorDstSelected(); break;
        case 23: _t->OnPipelineColorOperationSelected(); break;
        case 24: _t->OnPipelineAlphaSrcSelected(); break;
        case 25: _t->OnPipelineAlphaDstSelected(); break;
        case 26: _t->OnPipelineAlphaOperationSelected(); break;
        case 27: _t->OnPipelineDepthFormatSelected(); break;
        case 28: _t->OnSceneGeometryModeSelected(); break;
        case 29: _t->OnEnableResize(); break;
        case 30: _t->OnResolutionSelected(); break;
        case 31: _t->OnExtentXChanged(); break;
        case 32: _t->OnExtentYChanged(); break;
        case 33: _t->OnLayersChanged(); break;
        case 34: _t->OnBufferNameChanged(); break;
        case 35: _t->OnBufferSizeChanged(); break;
        case 36: _t->OnBufferUsageSelected(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Razix::Editor::RZEFrameGraphEditor::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_Razix__Editor__RZEFrameGraphEditor.data,
    qt_meta_data_Razix__Editor__RZEFrameGraphEditor,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Razix::Editor::RZEFrameGraphEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Razix::Editor::RZEFrameGraphEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Razix__Editor__RZEFrameGraphEditor.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int Razix::Editor::RZEFrameGraphEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 37)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 37;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 37)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 37;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
