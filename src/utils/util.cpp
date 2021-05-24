#include "util.h"

#include <Windows.h>
#include <dbghelp.h>

#include <array>

#include <QFileInfo>
#include <QWidget>
#include <QStringList>
#include <QFileInfo>
#include <QDir>
#include <QProcess>
#include <QUrl>
#include <QDesktopServices>
#include <QMessageBox>
#include <QMap>
#include <QDoubleSpinBox>
#include <QTemporaryFile>
#include <QApplication>
#include <QCryptographicHash>
#include <QtGlobal>
#include <QDesktopWidget>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

#ifdef Q_OS_MAC
static const unsigned int kLowMemoryThresholdPercent = 10U;
#else
static const unsigned int kLowMemoryThresholdKB = 256U * 1024U;
#endif

void Util::printStackTrace() {
    HANDLE process = GetCurrentProcess();
    HANDLE thread = GetCurrentThread();
    
    CONTEXT context;
    memset(&context, 0, sizeof(context));
    context.ContextFlags = CONTEXT_FULL;
    
    RtlCaptureContext(&context);
    SymInitialize(process, NULL, true);
    
    DWORD image;
    STACKFRAME64 stackframe;
    ZeroMemory(&stackframe, sizeof(STACKFRAME64));
    
    image = IMAGE_FILE_MACHINE_AMD64;
    stackframe.AddrPC.Offset    = context.Rip;
    stackframe.AddrPC.Mode      = AddrModeFlat;
    stackframe.AddrFrame.Offset = context.Rsp;
    stackframe.AddrFrame.Mode   = AddrModeFlat;
    stackframe.AddrStack.Offset = context.Rsp;
    stackframe.AddrStack.Mode   = AddrModeFlat;
    
    for (size_t i = 0; i < 25; i++) {
        bool result = StackWalk64(
            image, process, thread,
            &stackframe, &context,
            nullptr,
            SymFunctionTableAccess64, SymGetModuleBase64,
            nullptr
        );
        
        if (!result) {
            break;
        }
        
        static std::array<char, sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)> buffer;
        PSYMBOL_INFO symbol = reinterpret_cast<PSYMBOL_INFO>(buffer.data());
        symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        symbol->MaxNameLen = MAX_SYM_NAME;
        
        DWORD64 displacement = 0;
        if (SymFromAddr(process, stackframe.AddrPC.Offset, &displacement, symbol)) {
            printf("[%i] 0x%x %s\n", i, symbol->Address, symbol->Name);
        } else {
            printf("[%i] 0x%x ???\n", i, symbol->Address, symbol->Name);
        }
    }
    
    SymCleanup(process);
}

QString Util::baseName(const QString &filePath) {
    QString s = filePath;
    // Only if absolute path and not a URI.
    if (s.startsWith('/') || s.midRef(1, 2) == ":/" || s.midRef(1, 2) == ":\\")
        s = QFileInfo(s).fileName();
    return s;
}

void Util::setColorsToHighlight(QWidget* widget, QPalette::ColorRole role) {
    if (role == QPalette::Base) {
        widget->setStyleSheet(
                    "QLineEdit {"
                        "font-weight: bold;"
                        "background-color: palette(highlight);"
                        "color: palette(highlighted-text);"
                        "selection-background-color: palette(alternate-base);"
                        "selection-color: palette(text);"
                    "}"
                    "QLineEdit:hover {"
                        "border: 2px solid palette(button-text);"
                    "}"
        );
    } else {
        QPalette palette = QApplication::palette();
        palette.setColor(role, palette.color(palette.Highlight));
        palette.setColor(role == QPalette::Button ? QPalette::ButtonText : QPalette::WindowText,
            palette.color(palette.HighlightedText));
        widget->setPalette(palette);
        widget->setAutoFillBackground(true);
    }
}

void Util::showInFolder(const QString& path) {
    QFileInfo info(path);
#if defined(Q_OS_WIN)
    QStringList args;
    if (!info.isDir())
        args << "/select,";
    args << QDir::toNativeSeparators(path);
    if (QProcess::startDetached("explorer", args))
        return;
#elif defined(Q_OS_MAC)
    QStringList args;
    args << "-e";
    args << "tell application \"Finder\"";
    args << "-e";
    args << "activate";
    args << "-e";
    args << "select POSIX file \"" + path + "\"";
    args << "-e";
    args << "end tell";
#if !defined(QT_DEBUG)
    args << "-e";
    args << "return";
#endif
    if (!QProcess::execute("/usr/bin/osascript", args))
        return;
#endif
    QDesktopServices::openUrl(QUrl::fromLocalFile(info.isDir()? path : info.path()));
}

bool Util::warnIfNotWritable(const QString& filePath, QWidget* parent, const QString& caption) {
    // Returns true if not writable.
    if (!filePath.isEmpty() && !filePath.contains("://")) {
        QFileInfo info(filePath);
        if (!info.isDir()) {
            info = QFileInfo(info.dir().path());
        }
        if (!info.isWritable()) {
            info = QFileInfo(filePath);
            QMessageBox::warning(parent, caption,
                                 QObject::tr("Unable to write file %1\n"
                                    "Perhaps you do not have permission.\n"
                                    "Try again with a different folder.")
                                 .arg(info.fileName()));
            return true;
        }
    }
    return false;
}

QString Util::removeFileScheme(QUrl& url) {
    QString path = url.url();
    if (url.scheme() == "file")
        path = url.toString(QUrl::PreferLocalFile);
    return QUrl::fromPercentEncoding(path.toUtf8());
}

static inline bool isValidGoProFirstFilePrefix(const QFileInfo& info) {
    QStringList list {"GOPR", "GH01", "GS01", "GX01"};
    return list.contains(info.baseName().left(4).toUpper());
}

static inline bool isValidGoProPrefix(const QFileInfo& info) {
    QStringList list {"GP", "GH", "GS", "GX"};
    return list.contains(info.baseName().left(2).toUpper());
}

static inline bool isValidGoProSuffix(const QFileInfo& info) {
    QStringList list {"MP4", "LRV", "360", "WAV"};
    return list.contains(info.suffix().toUpper());
}

QStringList Util::sortedFileList(const QList<QUrl>& urls) {
    QStringList result;
    QMap<QString, QStringList> goproFiles;

    // First look for GoPro main files.
    foreach (QUrl url, urls) {
        QFileInfo fi(removeFileScheme(url));
        if (fi.baseName().size() == 8 && isValidGoProSuffix(fi) && isValidGoProFirstFilePrefix(fi)) {
            goproFiles[fi.baseName().mid(4)] << fi.filePath();
        }
    }
    // Then, look for GoPro split files.
    foreach (QUrl url, urls) {
        QFileInfo fi(removeFileScheme(url));
        if (fi.baseName().size() == 8 && isValidGoProSuffix(fi) && isValidGoProPrefix(fi) && !isValidGoProFirstFilePrefix(fi)) {
            QString goproNumber = fi.baseName().mid(4);
            // Only if there is a matching main GoPro file.
            if (goproFiles.contains(goproNumber) && goproFiles[goproNumber].size()) {
                goproFiles[goproNumber] << fi.filePath();
            }
        }
    }
    // Next, sort the GoPro files.
    foreach (QString goproNumber, goproFiles.keys())
        goproFiles[goproNumber].sort(Qt::CaseSensitive);
    // Finally, build the list of all files.
    // Add all the GoPro files first.
    foreach (QStringList paths, goproFiles)
        result << paths;
    // Add all the non-GoPro files.
    foreach (QUrl url, urls) {
        QFileInfo fi(removeFileScheme(url));
        if (fi.baseName().size() == 8 && isValidGoProSuffix(fi) &&
                (isValidGoProFirstFilePrefix(fi) || isValidGoProPrefix(fi))) {
            QString goproNumber = fi.baseName().mid(4);
            if (goproFiles.contains(goproNumber) && goproFiles[goproNumber].contains(fi.filePath()))
                continue;
        }
        result << fi.filePath();
    }
    return result;
}

int Util::coerceMultiple(int value, int multiple) {
    return (value + multiple - 1) / multiple * multiple;
}

QList<QUrl> Util::expandDirectories(const QList<QUrl>& urls) {
    QList<QUrl> result;
    foreach (QUrl url, urls) {
        QString path = Util::removeFileScheme(url);
        QFileInfo fi(path);
        if (fi.isDir()) {
            QDir dir(path);
            foreach (QFileInfo fi, dir.entryInfoList(QDir::Files | QDir::Readable, QDir::Name))
                result << QUrl(fi.filePath());
        } else {
            result << url;
        }
    }
    return result;
}

bool Util::isDecimalPoint(QChar ch) {
    // See https://en.wikipedia.org/wiki/Decimal_separator#Unicode_characters
    return ch == '.' || ch == ',' || ch == '\'' || ch == ' '
        || ch == QChar(0x00B7) || ch == QChar(0x2009) || ch == QChar(0x202F)
        || ch == QChar(0x02D9) || ch == QChar(0x066B) || ch == QChar(0x066C)
        || ch == QChar(0x2396);
}

bool Util::isNumeric(QString& str) {
    for (int i = 0; i < str.size(); ++i) {
        QCharRef ch = str[i];
        if (ch != '+' && ch != '-' && ch.toLower() != 'e'
            && !isDecimalPoint(ch) && !ch.isDigit())
            return false;
    }
    return true;
}

bool Util::convertNumericString(QString& str, QChar decimalPoint) {
    // Returns true if the string was changed.
    bool result = false;
    if (isNumeric(str)) {
        for (int i = 0; i < str.size(); ++i) {
            QCharRef ch = str[i];
            if (ch != decimalPoint && isDecimalPoint(ch)) {
                ch = decimalPoint;
                result = true;
            }
        }
    }
    return result;
}

bool Util::convertDecimalPoints(QString& str, QChar decimalPoint) {
    // Returns true if the string was changed.
    bool result = false;
    if (!str.contains(decimalPoint)) {
        for (int i = 0; i < str.size(); ++i) {
            QCharRef ch = str[i];
            // Space is used as a delimiter for rect fields and possibly elsewhere.
            if (ch != decimalPoint && ch != ' ' && isDecimalPoint(ch)) {
                ch = decimalPoint;
                result = true;
            }
        }
    }
    return result;
}

void Util::showFrameRateDialog(const QString& caption, int numerator, QDoubleSpinBox* spinner, QWidget *parent) {
    double fps = numerator / 1001.0;
    QMessageBox dialog(QMessageBox::Question, caption,
                       QObject::tr("The value you entered is very similar to the common,\n"
                          "more standard %1 = %2/1001.\n\n"
                          "Do you want to use %1 = %2/1001 instead?")
                          .arg(fps, 0, 'f', 6).arg(numerator),
                       QMessageBox::No | QMessageBox::Yes,
                       parent);
    dialog.setDefaultButton(QMessageBox::Yes);
    dialog.setEscapeButton(QMessageBox::No);
    dialog.setWindowModality(Qt::WindowModality::WindowModal);
    if (dialog.exec() == QMessageBox::Yes) {
        spinner->setValue(fps);
    }
}

QTemporaryFile* Util::writableTemporaryFile(const QString& filePath, const QString& templateName)
{
    // filePath should already be checked writable.
    QFileInfo info(filePath);
    QString templateFileName = templateName.isEmpty()?
        QString("%1.XXXXXX").arg(QCoreApplication::applicationName()) : templateName;

    // First, try the system temp dir.
    QString templateFilePath = QDir(QDir::tempPath()).filePath(templateFileName);
    QScopedPointer<QTemporaryFile> tmp(new QTemporaryFile(templateFilePath));

    if (!tmp->open() || tmp->write("") < 0) {
        // Otherwise, use the directory provided.
        return new QTemporaryFile(info.dir().filePath(templateFileName));
    } else {
        return tmp.take();
    }
}

QString Util::getFileHash(const QString& path) {
    // This routine is intentionally copied from Kdenlive.
    QFile file(path);
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray fileData;
         // 1 MB = 1 second per 450 files (or faster)
         // 10 MB = 9 seconds per 450 files (or faster)
        if (file.size() > 1000000*2) {
            fileData = file.read(1000000);
            if (file.seek(file.size() - 1000000))
                fileData.append(file.readAll());
        } else {
            fileData = file.readAll();
        }
        file.close();
        return QCryptographicHash::hash(fileData, QCryptographicHash::Md5).toHex();
    }
    return QString();
}

bool Util::hasDriveLetter(const QString& path) {
    auto driveSeparators = path.midRef(1, 2);
    return driveSeparators == ":/" || driveSeparators == ":\\";
}

QFileDialog::Options Util::getFileDialogOptions() {
#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
    if (qEnvironmentVariableIsSet("SNAP")) {
        return QFileDialog::DontUseNativeDialog;
    }
#endif
    return QFileDialog::Options();
}

#include <QDebug>

bool Util::isMemoryLow() {
#if defined(Q_OS_WIN)
    unsigned int availableKB = UINT_MAX;
    MEMORYSTATUSEX memory_status;
    ZeroMemory(&memory_status, sizeof(MEMORYSTATUSEX));
    memory_status.dwLength = sizeof(MEMORYSTATUSEX);
    if (GlobalMemoryStatusEx(&memory_status)) {
        availableKB = memory_status.ullAvailPhys / 1024UL;
    }
    qDebug() << "available RAM = " << availableKB << "KB";
    return availableKB < kLowMemoryThresholdKB;
#elif defined(Q_OS_MAC)
    QProcess p;
    p.start("memory_pressure");
    p.waitForFinished();
    auto lines = p.readAllStandardOutput();
    p.close();
    for (const auto& line : lines.split('\n')) {
        if (line.startsWith("System-wide memory free")) {
            auto fields = line.split(':');
            for (auto s : fields) {
                bool ok = false;
                auto percentage = s.replace('%', "").toUInt(&ok);
                if (ok) {
                    LOG_INFO() << percentage << '%';
                    return percentage <= kLowMemoryThresholdPercent;
                }
            }
        }
    }
    return false;
#elif defined(Q_OS_LINUX)
    unsigned int availableKB = UINT_MAX;
    QFile meminfo("/proc/meminfo");
    if (meminfo.open(QIODevice::ReadOnly)) {
        for (auto line = meminfo.readLine(1024);
             availableKB == UINT_MAX && !line.isEmpty();
             line = meminfo.readLine(1024)
        ) {
            if (line.startsWith("MemAvailable")) {
                auto fields = line.split(' ');
                for (const auto& s : fields) {
                    bool ok = false;
                    auto kB = s.toUInt(&ok);
                    if (ok) {
                        availableKB = kB;
                        break;
                    }
                }
            }
        }
    }
    meminfo.close();
    qDebug() << "available RAM = " << availableKB << "KB";
    return availableKB < kLowMemoryThresholdKB;
#endif
}

QString Util::getRandomCacheFilename() {
    return QString();
}

#include <QDate>

QString Util::getDateBasedPathString() {
    return QDate::currentDate().toString("yyyy-MM-dd/");
}

QString Util::getTimeBasedFilename(const QString& suffix) {
    return QTime::currentTime().toString("hh_mm_ss_z") + "." + suffix;
}

QString Util::convertColorString(
    const QColor&   color,
    const QString&  type
) {
    if(type == "RGBA") {
        return QString("rgba(%1, %2, %3, %4)")
                .arg(color.red())
                .arg(color.green())
                .arg(color.blue())
                .arg(color.alpha());
    }
    if(type == "RGB") {
        return QString("rgba(%1, %2, %3)")
                .arg(color.red())
                .arg(color.green())
                .arg(color.blue());
    }
    if(type == "HEX") {
        return color.name(QColor::HexArgb);
    }

    return color.name();
}

void Util::createFile(QString filePath, QString fileName) {
    QDir tempDir;
    
    QString currentDir = tempDir.currentPath();
    if (!tempDir.exists(filePath)) {
        qDebug() << QApplication::tr("failed to find path:") << filePath;
        tempDir.mkpath(filePath);
    }
    QFile *tempFile = new QFile;

    
    tempDir.setCurrent(filePath);
    qDebug() << tempDir.currentPath();

    
    if (tempFile->exists(fileName)) {
        qDebug() << QApplication::tr("file already exists");
        return;
    }
    
    tempFile->setFileName(fileName);
    if (!tempFile->open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "failed to open file:" << fileName;
    }
    tempFile->close();

    tempDir.setCurrent(currentDir);
    qDebug() << tempDir.currentPath();
}

constexpr float WINDOW_BASESIZE_WIDTH  = 1920.0f;
constexpr float WINDOW_BASESIZE_HEIGHT = 1080.0f;

// 考虑到多屏幕，需要使用单一屏幕大小
float Util::getWindowWidthMultiplyingPower() {
    auto&& temp = QApplication::desktop()->size();
    return static_cast<float>(temp.width()) / WINDOW_BASESIZE_WIDTH;
}
float Util::getWindowHeightMultiplyingPower() {
    auto&& temp = QApplication::desktop()->size();
    return static_cast<float>(temp.height()) / WINDOW_BASESIZE_HEIGHT;
}
