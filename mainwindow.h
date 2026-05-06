#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QVector>
#include <QString>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

private slots:
    void onSubmit();
    void onClear();
    void onInputChanged();

private:
    struct Question {
        QString text;
        QString placeholder;
        bool    isNumeric;
        int     minVal;
        int     maxVal;
    };

    void setupQuestions();
    void setupUI();
    void applyStyles();
    bool validateAll();
    void saveToFile();
    void showResult(bool success, const QString &filePath);

    QVector<Question>   m_questions;
    QVector<QLineEdit*> m_inputs;
    QVector<QLabel*>    m_errorLabels;

    QPushButton *m_submitBtn;
    QPushButton *m_clearBtn;
    QLabel      *m_statusLabel;
};

#endif // MAINWINDOW_H
