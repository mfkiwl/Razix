/********************************************************************************
** Form generated from reading UI file 'RZEInspectorWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RZEINSPECTORWINDOW_H
#define UI_RZEINSPECTORWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_InspectorWindow
{
public:
    QVBoxLayout *verticalLayout_2;
    QSpacerItem *verticalSpacer;

    void setupUi(QWidget *InspectorWindow)
    {
        if (InspectorWindow->objectName().isEmpty())
            InspectorWindow->setObjectName(QString::fromUtf8("InspectorWindow"));
        InspectorWindow->resize(401, 486);
        InspectorWindow->setStyleSheet(QString::fromUtf8(""));
        verticalLayout_2 = new QVBoxLayout(InspectorWindow);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer);


        retranslateUi(InspectorWindow);

        QMetaObject::connectSlotsByName(InspectorWindow);
    } // setupUi

    void retranslateUi(QWidget *InspectorWindow)
    {
        InspectorWindow->setWindowTitle(QCoreApplication::translate("InspectorWindow", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class InspectorWindow: public Ui_InspectorWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RZEINSPECTORWINDOW_H
