#ifndef UTIL_H
#define UTIL_H

#include <QWidget>
#include <QDoubleSpinBox>
#include <QTemporaryFile>
#include <QFileDialog>

class Util {
     Util() = delete;
	~Util() = delete;
public:
	static void printStackTrace();
    static QString baseName(const QString &filePath);
    static void setColorsToHighlight(QWidget* widget, QPalette::ColorRole role = QPalette::Window);
    static void showInFolder(const QString &path);
    static bool warnIfNotWritable(const QString& filePath, QWidget* parent, const QString& caption);
    static QString removeFileScheme(QUrl &url);
    static QStringList sortedFileList(const QList<QUrl>& urls);
    static int coerceMultiple(int value, int multiple = 2);
    static QList<QUrl> expandDirectories(const QList<QUrl>& urls);
    static bool isDecimalPoint(QChar ch);
    static bool isNumeric(QString& str);
    static bool convertNumericString(QString& str, QChar decimalPoint);
    static bool convertDecimalPoints(QString& str, QChar decimalPoint);
    static void showFrameRateDialog(const QString& caption, int numerator, QDoubleSpinBox* spinner, QWidget* parent = Q_NULLPTR);
    static QTemporaryFile* writableTemporaryFile(const QString& filePath = QString(), const QString& templateName = QString());
    static QString getFileHash(const QString& path);
    static bool hasDriveLetter(const QString& path);
    static QFileDialog::Options getFileDialogOptions();
    static bool isMemoryLow();
    static QString getRandomCacheFilename();
    static QString getDateBasedPathString();
    static QString getTimeBasedFilename(const QString& suffix);
    static QString convertColorString(const QColor &color, const QString& type = "HEX");
    static void createFile(QString filePath, QString fileName);
    static float getWindowWidthMultiplyingPower();
    static float getWindowHeightMultiplyingPower(); 
};

#endif // UTIL_H
