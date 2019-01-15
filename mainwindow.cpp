/**************************************************************************************
 **                                                                                  **
 **   Copyright (C) 2012 ANDRÉ Guillaume <guillaume.andre.dev@gmail.com>             **
 **                                                                                  **
 **   This program free software; you can redistribute it and/or                     **
 **   modify it under the terms of the GNU Lesser General Public                     **
 **   License as published by the Free Software Foundation; either                   **
 **   version 2.1 of the License, or (at your option) any later version.             **
 **                                                                                  **
 **   This program is distributed in the hope that it will be useful,                **
 **   but WITHOUT ANY WARRANTY; without even the implied warranty of                 **
 **   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU               **
 **   Lesser General Public License for more details.                                **
 **                                                                                  **
 **   You should have received a copy of the GNU Lesser General Public               **
 **   License along with this program; if not, write to the Free Software            **
 **   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA   **
 **                                                                                  **
 **************************************************************************************/


#include <QBoxLayout>
#include <QCheckBox>
#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QStatusBar>
#include <QtConcurrent/QtConcurrentRun>

#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_sumList(QStringList() << "MD4" << "MD5" << "SHA1" << "SHA-224" << "SHA-256" << "SHA-384"
                              << "SHA-512" << "SHA3-224" << "SHA3-256" << "SHA3-384" << "SHA3-512"),
      m_filePath(new QLineEdit()),
      m_calculate(new QPushButton(tr("Calculate"))),
      m_status(new QLabel(tr("Ready")))
{
    setCentralWidget(new QWidget());
    setWindowTitle("Sum checker");

    QVBoxLayout *mainLayout = new QVBoxLayout;

    QHBoxLayout *searchLayout = new QHBoxLayout;
    QLabel *searchLabel = new QLabel(tr("File path : "));
    QPushButton *search = new QPushButton(tr("Search..."));

    searchLayout->addWidget(searchLabel);
    searchLayout->addWidget(m_filePath);
    searchLayout->addWidget(search);

    QFormLayout *sumLayout = new QFormLayout;
    for (int i(0) ; i < 11 ; ++i) {
        QCheckBox *box = new QCheckBox(m_sumList.at(i) + " :");
        box->setChecked(true);
        m_sums[i] = new QLineEdit;
        m_sums[i]->setReadOnly(true);
        connect(box, &QCheckBox::clicked, m_sums[i], &QLineEdit::setEnabled);
        sumLayout->addRow(box, m_sums[i]);
    }

    QHBoxLayout *compareLayout = new QHBoxLayout;
    m_compare = new QLineEdit;
    QPushButton *compareButton = new QPushButton(tr("Compare"));
    compareLayout->addWidget(m_compare);
    compareLayout->addWidget(compareButton);

    mainLayout->addLayout(searchLayout);
    mainLayout->addSpacing(30);
    mainLayout->addLayout(sumLayout);
    mainLayout->addWidget(m_calculate, 0, Qt::AlignCenter);
    mainLayout->addLayout(compareLayout);
    centralWidget()->setLayout(mainLayout);

    statusBar()->addPermanentWidget(m_status);

    connect(m_filePath, &QLineEdit::textChanged, this, &MainWindow::clearSums);
    connect(search, &QPushButton::clicked, this, &MainWindow::searchFile);
    connect(m_calculate, &QPushButton::clicked, this, &MainWindow::prepareReading);
    connect(compareButton, &QPushButton::clicked, this, &MainWindow::compareSums);
}


MainWindow::~MainWindow()
{ }


void MainWindow::readFile()
{
    QVariant result;

    QFile file(m_filePath->text());
    if (!file.exists()) {
        result = 1;
    }
    if (!file.open(QIODevice::ReadOnly)) {
        result = 2;
    }
    result = file.readAll();
    file.close();
    calculateSums(result);
}


void MainWindow::calculateSums(const QVariant &data)
{
    if (static_cast<QMetaType::Type>(data.type()) != QMetaType::QByteArray) {
        if (data.toInt() == 1) {
            QMessageBox::critical(this, tr("Error"), tr("The file does not exist."));
        } else {
            QMessageBox::critical(this, tr("Error"), tr("Cannot open the file for reading."));
        }
    }

    for (int i(0) ; i < 11 ; ++i) {
        if (m_sums[i]->isEnabled()) {
            m_status->setText(QString("Calculating %1 sum...").arg(m_sumList.at(i)));
            m_sums[i]->setText(QCryptographicHash::hash(data.toByteArray(),
                                                        static_cast<QCryptographicHash::Algorithm>(i)).toHex());
        }
    }

    centralWidget()->layout()->itemAt(0)->layout()->itemAt(0)->widget()->setEnabled(true);
    centralWidget()->layout()->itemAt(0)->layout()->itemAt(1)->widget()->setEnabled(true);
    centralWidget()->layout()->itemAt(0)->layout()->itemAt(2)->widget()->setEnabled(true);

    m_calculate->setEnabled(true);
    m_status->setText(tr("Ready"));
}


void MainWindow::clearSums()
{
    for (int i(0) ; i < 11 ; ++i) {
        m_sums[i]->clear();
    }

    QPalette palette;
    if (!QFile::exists(m_filePath->text()) && !m_filePath->text().isEmpty()) {
        palette.setColor(QPalette::Text, Qt::red);
    }
    m_filePath->setPalette(palette);
}


void MainWindow::compareSums()
{
    if (m_compare->text().isEmpty()) {
        QMessageBox::critical(this, tr("Error"), tr("The sum to compare is empty."));
        return;
    }

    QStringList sumsThatMatch;
    for (int i(0) ; i < 11 ; ++i) {
        if (m_sums[i]->text() == m_compare->text()) {
            sumsThatMatch.append(m_sumList.at(i));
        }
    }

    if (sumsThatMatch.size() == 0) {
        QMessageBox::information(this, "", tr("No match found for this sum."));
    } else if (sumsThatMatch.size() == 1) {
        QMessageBox::information(this, "", tr("Match found with %1 sum.").arg(sumsThatMatch.join("")));
    } else {
        QMessageBox::information(this, "", tr("Matches found with %1 sums.").arg(sumsThatMatch.join(", ")));
    }
}


void MainWindow::prepareReading()
{
    m_calculate->setEnabled(false);
    centralWidget()->layout()->itemAt(0)->layout()->itemAt(0)->widget()->setEnabled(false);
    centralWidget()->layout()->itemAt(0)->layout()->itemAt(1)->widget()->setEnabled(false);
    centralWidget()->layout()->itemAt(0)->layout()->itemAt(2)->widget()->setEnabled(false);
    m_status->setText(tr("Reading file..."));
    QtConcurrent::run(this, &MainWindow::readFile);
}


void MainWindow::searchFile()
{
    const QString path(QFileDialog::getOpenFileName(this, tr("Select a file")));
    if (!path.isEmpty()) {
        m_filePath->setText(path);
    }
}



















