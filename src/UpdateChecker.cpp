
#include "UpdateChecker.h"

//=== Constructor/Destructor ===//

/* Constructor
 * @arg url - URL to query for an updated program version number
 * @arg currentVersion - current version number of this program
 */
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

/* Destructor */
UpdateChecker::~UpdateChecker() {
    delete _netManager;
}

//=== Accessors ===//

/* Finished
 * True if the check has been completed
 */
bool UpdateChecker::finished() const {
    return _finished;
}

/* Has major update
 * True if a non-minor version change has been made
 * (Numbers before the first two dots have changed)
 */
bool UpdateChecker::hasUpdate() const {
    return _hasUpdate;
}

/* Has minor update
 * True if any version change has been made
 */
bool UpdateChecker::hasMinorUpdate() const {
    return _hasMinorUpdate;
}

/* Errors returned by the network request
 */
QNetworkReply::NetworkError UpdateChecker::error() const {
    return _error;
}

/* HTTP Response text from the version check
 * Should contain the updated version number of the program
 * (Could also be helpful in determining causes of errors)
 */
const QByteArray & UpdateChecker::httpResponse() const {
    return _httpResponse;
}

//=== Mutators ===//

/* Query the already-specified URL to determine if any
 * version updates have been made to the program.
 * Fires requestFinished() on completion.
 */
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

/* Called when the Network Manager has finished its request
 */
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
    // Emit a signal so that users know that the request has
    // finished, and give them a pointer to this object
    emit checkFinished(this);
}
