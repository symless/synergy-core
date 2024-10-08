/*
 * Deskflow -- mouse and keyboard sharing utility
 * Copyright (C) 2012 Symless Ltd.
 *
 * This package is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * found in the file LICENSE that should have accompanied this file.
 *
 * This package is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "VersionChecker.h"

#include "common/constants.h"
#include "env_vars.h"

#include <QLocale>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QProcess>
#include <QRegularExpression>
#include <memory>

using namespace deskflow::gui;

VersionChecker::VersionChecker(std::shared_ptr<QNetworkAccessManagerProxy> network)
    : m_network(network ? network : std::make_shared<QNetworkAccessManagerProxy>())
{
    m_network->init();
    connect(m_network.get(), &QNetworkAccessManagerProxy::finished, this, &VersionChecker::replyFinished);
}

void VersionChecker::checkLatest() const
{
    const QString url = env_vars::versionUrl();
    qDebug("checking for updates at: %s", qPrintable(url));
    auto request = QNetworkRequest(url);
    auto userAgent = QString("%1 %2 on %3").arg(kAppName).arg(kVersion).arg(QSysInfo::prettyProductName());
    request.setHeader(QNetworkRequest::UserAgentHeader, userAgent);
    request.setRawHeader("X-" DESKFLOW_APP_NAME "-Version", kVersion);
    request.setRawHeader("X-" DESKFLOW_APP_NAME "-Language", QLocale::system().name().toStdString().c_str());
    m_network->get(request);
}

void VersionChecker::replyFinished(QNetworkReply *reply)
{
    auto newestVersion = QString(reply->readAll());
    if (!newestVersion.isEmpty() && compareVersions(DESKFLOW_VERSION, newestVersion) > 0) {
        qDebug("update found: %s", qPrintable(newestVersion));
        emit updateFound(newestVersion);
    } else {
        qDebug("no updates found");
    }
}

int VersionChecker::getStageVersion(QString stage)
{
    const char *stableName = "stable";
    const char *rcName = "rc";
    const char *betaName = "beta";

    // use max int for stable so it's always the highest value.
    const int stableValue = INT_MAX;
    const int rcValue = 2;
    const int betaValue = 1;
    const int otherValue = 0;

    if (stage.isEmpty() || stage == stableName) {
        return stableValue;
    } else if (stage.toLower().startsWith(rcName)) {
        QRegularExpression re("\\d*", QRegularExpression::CaseInsensitiveOption);
        if (re.match(stage).hasMatch()) {
            // return the rc value plus the rc number (e.g. 2 + 1)
            // this should be ok since stable is max int.
            return rcValue + re.match(stage).captured(1).toInt();
        }
    } else if (stage == betaName) {
        return betaValue;
    }

    return otherValue;
}

int VersionChecker::compareVersions(const QString &left, const QString &right)
{
    if (left.compare(right) == 0)
        return 0; // versions are same.

    QStringList leftParts = left.split("-");
    QStringList rightParts = right.split("-");

    QString leftNumber = leftParts.at(0);
    QString rightNumber = rightParts.at(0);

    QStringList leftNumberParts = left.split(".");
    QStringList rightNumberParts = right.split(".");

    auto leftStagePart = leftParts.size() > 1 ? leftParts.at(1) : "";
    auto rightStagePart = rightParts.size() > 1 ? rightParts.at(1) : "";

    const int leftMajor = leftNumberParts.at(0).toInt();
    const int leftMinor = leftNumberParts.at(1).toInt();
    const int leftPatch = leftNumberParts.at(2).toInt();
    const int leftStage = getStageVersion(leftStagePart);

    const int rightMajor = rightNumberParts.at(0).toInt();
    const int rightMinor = rightNumberParts.at(1).toInt();
    const int rightPatch = rightNumberParts.at(2).toInt();
    const int rightStage = getStageVersion(rightStagePart);

    const bool rightWins = (rightMajor > leftMajor) || ((rightMajor >= leftMajor) && (rightMinor > leftMinor))
        || ((rightMajor >= leftMajor) && (rightMinor >= leftMinor) && (rightPatch > leftPatch))
        || ((rightMajor >= leftMajor) && (rightMinor >= leftMinor) && (rightPatch >= leftPatch) && (rightStage > leftStage));

    return rightWins ? 1 : -1;
}
