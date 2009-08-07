#ifndef V_UPDATE_CHECKER_H
#define V_UPDATE_CHECKER_H

#include <QtCore>
#include <QtNetwork>

/* UpdateChecker: Utility for making HTTP queries to
 * check the current program's version against the
 * current version on a specified web server
 * The web server response should be plain-text,
 * containing only the version number.
 * Version numbers must be in the format
 * [0-9]+.[0-9]+.[0-9]+
 */
class UpdateChecker : public QObject {

Q_OBJECT // Qt Library Macro

public:

    //=== Constructor/Destructor ===//

    /* Constructor
     * @arg url - URL to query for an updated program version number
     * @arg currentVersion - current version number of this program
     */
    UpdateChecker(const char * url, const QString & currentVersion);

    /* Destructor */
    ~UpdateChecker();

    //=== Accessors ===//

    /* Finished
     * True if the check has been completed
     */
    bool finished() const;

    /* Has major update
     * True if a non-minor version change has been made
     * (Numbers before the first two dots have changed)
     */
    bool hasUpdate() const;

    /* Has minor update
     * True if any version change has been made
     */
    bool hasMinorUpdate() const;

    /* Errors returned by the network request
     */
    QNetworkReply::NetworkError error() const;

    /* HTTP Response text from the version check
     * Should contain the updated version number of the program
     * (Could also be helpful in determining causes of errors)
     */
    const QByteArray & httpResponse() const;

    //=== Mutators ===//

    /* Query the already-specified URL to determine if any
     * version updates have been made to the program.
     * Fires requestFinished() on completion.
     */
    void check();

private:

    // URL to query for new version info
    QUrl _url;

    // The 3 sub-parts of the current version number:
    // _v1._v2._v3
    int _v1, _v2, _v3;

    // Errors returned from the HTTP request
    QNetworkReply::NetworkError _error;

    // Regular expression for parsing the version string
    QRegExp versionExp;

    // Flags for query results
    bool _finished, _hasUpdate, _hasMinorUpdate;

    // Query response string
    // (Should contain the new version number)
    QByteArray _httpResponse;

    // Network Manager used to perform the query
    QNetworkAccessManager * _netManager;

signals:

    /* Signal fired upon completion of the version check
     */
    void checkFinished(const UpdateChecker * updateChecker);

private slots:

    //=== Network Action Slots ===//

    /* Called when the Network Manager has finished its request
     */
    void requestFinished(QNetworkReply * reply);

};


#endif
