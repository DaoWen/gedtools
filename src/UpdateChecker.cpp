
#include "UpdateChecker.h"

//=== Constructor/Destructor ===//

UpdateChecker::UpdateChecker(const char * url, const QString & currentVersion)
 : _url(url), versionExp("(\\d+)\\.(\\d+)\\.(\\d+)") {
    // Get the current version information
    versionExp.exactMatch(currentVersion);
    _v1 = versionExp.cap(1).toInt();
    _v2 = versionExp.cap(2).toInt();
    _v3 = versionExp.cap(3).toInt();
    _finished = false;
    _netManager = 0;
}

UpdateChecker::~UpdateChecker() {
    delete _netManager;
}

//=== Accessors ===//

bool UpdateChecker::finished() const {
    return _finished;
}


bool UpdateChecker::hasUpdate() const {
    return _hasUpdate;
}

bool UpdateChecker::hasMinorUpdate() const {
    return _hasMinorUpdate;
}

QNetworkReply::NetworkError UpdateChecker::error() const {
    return _error;
}

const QByteArray & UpdateChecker::httpResponse() const {
    return _httpResponse;
}

//=== Mutators ===//

void UpdateChecker::check() {
    // Only make the check if there hasn't been one already
    if (!_netManager) {
        // Initialize the Network Access Manager
        _netManager = new QNetworkAccessManager();
        // Call requestFinished() when the request is done
        connect(_netManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(requestFinished(QNetworkReply*)));
        // Make the request
        _netManager->get(QNetworkRequest(_url));
    }
}

//=== Network Action Slots ===//

void UpdateChecker::requestFinished(QNetworkReply * reply) {
    _httpResponse = reply->readAll();
    _error = reply->error();
    // Compare version numbers
    versionExp.indexIn(_httpResponse);
    int nv1 = versionExp.cap(1).toInt();
    int nv2 = versionExp.cap(2).toInt();
    int nv3 = versionExp.cap(3).toInt();
    _hasUpdate = (_v1 < nv1) || (_v1 == nv1) && (_v2 < nv2);
    _hasMinorUpdate = _hasUpdate || (_v2 == nv2) && (_v3 < nv3);
    // Finished!
    _finished = true;
    emit checkFinished(this);
}
