#include "mainwindow.h"

#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QWidget>
#include <QGroupBox>
#include <QIntValidator>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDir>
#include <QMessageBox>
#include <QFrame>
#include <QSizePolicy>

// ---------------------------------------------------------------------------
// Question definitions
// ---------------------------------------------------------------------------
void MainWindow::setupQuestions()
{
    m_questions = {
        { "1. Який ваш ігровий нікнейм (псевдонім)?",
          "Наприклад: ProGamer2000",
          false, 0, 0 },

        { "2. Скільки років ви граєте у відеоігри?",
          "Введіть число (0 – 80)",
          true, 0, 80 },

        { "3. Скільки годин на тиждень ви витрачаєте на ігри?",
          "Введіть число (0 – 168)",
          true, 0, 168 },

        { "4. Яка ваша улюблена ігрова платформа?",
          "Наприклад: PC, PlayStation, Xbox, Nintendo Switch",
          false, 0, 0 },

        { "5. Назвіть вашу улюблену гру всіх часів.",
          "Наприклад: The Witcher 3",
          false, 0, 0 },

        { "6. Скільки ігор є у вашій бібліотеці (Steam / інша платформа)?",
          "Введіть число (0 – 100000)",
          true, 0, 100000 },

        { "7. Яка найвища кількість очок (score), яку ви набрали в будь-якій грі?",
          "Введіть число (0 – 999999999)",
          true, 0, 999999999 },

        { "8. Оцініть свій ігровий рівень за шкалою від 1 до 10.",
          "Введіть число від 1 до 10",
          true, 1, 10 }
    };
}

// ---------------------------------------------------------------------------
// UI construction
// ---------------------------------------------------------------------------
void MainWindow::setupUI()
{
    setWindowTitle("Форма опитування — Ігрова тематика");
    setMinimumWidth(620);
    resize(680, 700);

    // --- central widget + outer layout ---
    QWidget     *central = new QWidget(this);
    QVBoxLayout *outerLayout = new QVBoxLayout(central);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->setSpacing(0);
    setCentralWidget(central);

    // --- header ---
    QLabel *header = new QLabel("🎮  Ігрове опитування", central);
    header->setObjectName("header");
    header->setAlignment(Qt::AlignCenter);
    header->setFixedHeight(70);
    outerLayout->addWidget(header);

    QLabel *subtitle = new QLabel(
        "Заповніть усі поля та натисніть «Надіслати». "
        "Відповіді буде збережено у файл.", central);
    subtitle->setObjectName("subtitle");
    subtitle->setAlignment(Qt::AlignCenter);
    subtitle->setWordWrap(true);
    outerLayout->addWidget(subtitle);

    // --- scroll area ---
    QScrollArea *scroll = new QScrollArea(central);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    outerLayout->addWidget(scroll, 1);

    QWidget     *scrollContent = new QWidget();
    QVBoxLayout *formLayout    = new QVBoxLayout(scrollContent);
    formLayout->setContentsMargins(24, 16, 24, 16);
    formLayout->setSpacing(12);
    scroll->setWidget(scrollContent);

    // --- questions ---
    for (int i = 0; i < m_questions.size(); ++i) {
        const Question &q = m_questions[i];

        QGroupBox   *box       = new QGroupBox(q.text, scrollContent);
        QVBoxLayout *boxLayout = new QVBoxLayout(box);
        boxLayout->setContentsMargins(12, 8, 12, 8);
        boxLayout->setSpacing(4);

        QLineEdit *input = new QLineEdit(box);
        input->setPlaceholderText(q.placeholder);
        input->setFixedHeight(36);
        input->setObjectName(q.isNumeric ? "numericInput" : "textInput");

        if (q.isNumeric) {
            auto *validator = new QIntValidator(q.minVal, q.maxVal, input);
            input->setValidator(validator);
        }

        QLabel *errLabel = new QLabel("", box);
        errLabel->setObjectName("errorLabel");
        errLabel->setVisible(false);

        boxLayout->addWidget(input);
        boxLayout->addWidget(errLabel);
        formLayout->addWidget(box);

        m_inputs.append(input);
        m_errorLabels.append(errLabel);

        connect(input, &QLineEdit::textChanged, this, &MainWindow::onInputChanged);
    }

    formLayout->addStretch();

    // --- status label ---
    m_statusLabel = new QLabel("", central);
    m_statusLabel->setObjectName("statusLabel");
    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->setWordWrap(true);
    m_statusLabel->setFixedHeight(32);
    outerLayout->addWidget(m_statusLabel);

    // --- buttons ---
    QWidget     *btnBar    = new QWidget(central);
    QHBoxLayout *btnLayout = new QHBoxLayout(btnBar);
    btnLayout->setContentsMargins(24, 8, 24, 16);
    btnLayout->setSpacing(12);

    m_clearBtn = new QPushButton("Очистити", btnBar);
    m_clearBtn->setObjectName("clearBtn");
    m_clearBtn->setFixedHeight(40);
    connect(m_clearBtn, &QPushButton::clicked, this, &MainWindow::onClear);

    m_submitBtn = new QPushButton("Надіслати відповіді", btnBar);
    m_submitBtn->setObjectName("submitBtn");
    m_submitBtn->setFixedHeight(40);
    connect(m_submitBtn, &QPushButton::clicked, this, &MainWindow::onSubmit);

    btnLayout->addWidget(m_clearBtn);
    btnLayout->addStretch();
    btnLayout->addWidget(m_submitBtn);
    outerLayout->addWidget(btnBar);
}

// ---------------------------------------------------------------------------
// Stylesheet
// ---------------------------------------------------------------------------
void MainWindow::applyStyles()
{
    qApp->setStyleSheet(R"(
        QMainWindow, QScrollArea, QWidget {
            background-color: #1e1e2e;
            color: #cdd6f4;
        }

        QLabel#header {
            font-size: 22px;
            font-weight: bold;
            color: #cba6f7;
            background-color: #181825;
            border-bottom: 2px solid #313244;
        }

        QLabel#subtitle {
            font-size: 11px;
            color: #a6adc8;
            padding: 6px 24px;
            background-color: #181825;
        }

        QGroupBox {
            font-size: 12px;
            font-weight: 600;
            color: #89b4fa;
            border: 1px solid #313244;
            border-radius: 8px;
            margin-top: 6px;
            padding-top: 10px;
            background-color: #181825;
        }

        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 4px;
        }

        QLineEdit#textInput, QLineEdit#numericInput {
            background-color: #313244;
            color: #cdd6f4;
            border: 1px solid #45475a;
            border-radius: 6px;
            padding: 6px 10px;
            font-size: 12px;
        }

        QLineEdit#textInput:focus, QLineEdit#numericInput:focus {
            border: 1px solid #89b4fa;
            background-color: #363649;
        }

        QLineEdit#numericInput {
            border-left: 3px solid #fab387;
        }

        QLabel#errorLabel {
            color: #f38ba8;
            font-size: 11px;
            padding-left: 4px;
        }

        QLabel#statusLabel {
            font-size: 12px;
        }

        QPushButton#submitBtn {
            background-color: #89b4fa;
            color: #1e1e2e;
            font-weight: bold;
            font-size: 13px;
            border: none;
            border-radius: 8px;
            padding: 0 24px;
            min-width: 180px;
        }

        QPushButton#submitBtn:hover  { background-color: #b4d0fe; }
        QPushButton#submitBtn:pressed { background-color: #74b2f5; }

        QPushButton#clearBtn {
            background-color: #45475a;
            color: #cdd6f4;
            font-size: 13px;
            border: none;
            border-radius: 8px;
            padding: 0 20px;
            min-width: 100px;
        }

        QPushButton#clearBtn:hover  { background-color: #585b70; }
        QPushButton#clearBtn:pressed { background-color: #3d3f50; }

        QScrollBar:vertical {
            background: #181825;
            width: 8px;
            margin: 0;
        }
        QScrollBar::handle:vertical {
            background: #45475a;
            border-radius: 4px;
            min-height: 30px;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
    )");
}

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupQuestions();
    setupUI();
    applyStyles();
}

// ---------------------------------------------------------------------------
// Validation
// ---------------------------------------------------------------------------
bool MainWindow::validateAll()
{
    bool allValid = true;

    for (int i = 0; i < m_questions.size(); ++i) {
        const Question &q     = m_questions[i];
        QLineEdit       *input = m_inputs[i];
        QLabel          *err   = m_errorLabels[i];
        QString          text  = input->text().trimmed();

        err->setVisible(false);

        if (text.isEmpty()) {
            err->setText("⚠  Поле не може бути порожнім.");
            err->setVisible(true);
            allValid = false;
            continue;
        }

        if (q.isNumeric) {
            bool ok = false;
            int  val = text.toInt(&ok);
            if (!ok || val < q.minVal || val > q.maxVal) {
                err->setText(QString("⚠  Введіть ціле число від %1 до %2.")
                                 .arg(q.minVal).arg(q.maxVal));
                err->setVisible(true);
                allValid = false;
            }
        }
    }

    return allValid;
}

// ---------------------------------------------------------------------------
// Save to file
// ---------------------------------------------------------------------------
void MainWindow::saveToFile()
{
    QString dir      = QDir::homePath() + "/SurveyResults";
    QString fileName = dir + "/survey_" +
                       QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss") +
                       ".txt";

    QDir().mkpath(dir);

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Помилка",
            "Не вдалося відкрити файл для запису:\n" + fileName);
        return;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);

    out << "=== Результати ігрового опитування ===\n";
    out << "Дата та час: "
        << QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss") << "\n";
    out << QString(40, '=') << "\n\n";

    for (int i = 0; i < m_questions.size(); ++i) {
        out << m_questions[i].text << "\n";
        out << "Відповідь: " << m_inputs[i]->text().trimmed() << "\n\n";
    }

    out << QString(40, '-') << "\n";
    out << "Кінець опитування\n";

    file.close();
    showResult(true, fileName);
}

void MainWindow::showResult(bool success, const QString &filePath)
{
    if (success) {
        m_statusLabel->setStyleSheet("color: #a6e3a1; font-weight: bold;");
        m_statusLabel->setText("✔  Відповіді збережено: " + filePath);

        QMessageBox::information(this, "Збережено",
            "Дякуємо за участь в опитуванні!\n\nФайл збережено:\n" + filePath);
    }
}

// ---------------------------------------------------------------------------
// Slots
// ---------------------------------------------------------------------------
void MainWindow::onSubmit()
{
    m_statusLabel->setText("");

    if (!validateAll()) {
        m_statusLabel->setStyleSheet("color: #f38ba8;");
        m_statusLabel->setText("⚠  Будь ласка, виправте помилки у виділених полях.");
        return;
    }

    saveToFile();
}

void MainWindow::onClear()
{
    for (QLineEdit *input : m_inputs)
        input->clear();

    for (QLabel *err : m_errorLabels)
        err->setVisible(false);

    m_statusLabel->setText("");
}

void MainWindow::onInputChanged()
{
    // Live-clear error for the field being edited
    QLineEdit *changed = qobject_cast<QLineEdit*>(sender());
    if (!changed) return;

    int idx = m_inputs.indexOf(changed);
    if (idx >= 0)
        m_errorLabels[idx]->setVisible(false);
}
