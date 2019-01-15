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


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QLabel;
class QLineEdit;
class QPushButton;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void readFile();

private slots:
    void calculateSums(const QVariant &data);
    void clearSums();
    void compareSums();
    void prepareReading();
    void searchFile();

private:
    QStringList m_sumList;

    QLineEdit *m_filePath;
    QLineEdit *m_sums[11];
    QPushButton *m_calculate;
    QLineEdit *m_compare;

    QLabel *m_status;
};

#endif // MAINWINDOW_H



















