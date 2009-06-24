#ifndef V_UPDATE_CHECKER_H
#define V_UPDATE_CHECKER_H

#include <QtCore>
#include <QtNetwork>

class UpdateChecker : public QObject {

Q_OBJECT // Qt Library Macro

public:

    //=== Constructor/Destructor ===//

    UpdateChecker(const char * url, const QString & currentVersion);

    ~UpdateChecker();

    //=== Accessors ===//

    bool finished() const;

    bool hasUpdate() const;

    bool hasMinorUpdate() const;

    QNetworkReply::NetworkError error() const;

    const QByteArray & httpResponse() const;

    //=== Mutators ===//

    void check();

private:

    QUrl _url;

    int _v1, _v2, _v3;

    QNetworkReply::NetworkError _error;

    QRegExp versionExp;

    bool _finished, _hasUpdate, _hasMinorUpdate;

    QByteArray _httpResponse;

    QNetworkAccessManager * _netManager;

signals:

    void checkFinished(const UpdateChecker * updateChecker);

private slots:

    //=== Network Action Slots ===//

    void requestFinished(QNetworkReply * reply);

};


#endif
