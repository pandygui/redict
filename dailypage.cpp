/*
 * Copyright (C) 2017 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     rekols <rekols@foxmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "dailypage.h"
#include <QScrollArea>
#include <QVBoxLayout>
#include <QTimer>

DailyPage::DailyPage(QWidget *parent)
    : QWidget(parent),
      m_networkManager(new QNetworkAccessManager(this)),
      m_imageLabel(new QLabel),
      m_titleLabel(new QLabel),
      m_summaryLabel(new QLabel),
      m_api(new YoudaoAPI)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QVBoxLayout *textLayout = new QVBoxLayout;

    m_imageLabel->setFixedHeight(250);
    m_imageLabel->setScaledContents(true);

    m_titleLabel->setWordWrap(true);
    m_summaryLabel->setWordWrap(true);

    textLayout->setContentsMargins(20, 10, 20, 10);
    textLayout->addWidget(m_titleLabel);
    textLayout->addWidget(m_summaryLabel);

    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(m_imageLabel);
    mainLayout->addLayout(textLayout);
    mainLayout->addStretch();

    QTimer::singleShot(100, m_api, &YoudaoAPI::queryDaily);

    connect(m_api, &YoudaoAPI::dailyFinished, this, &DailyPage::handleQueryFinished);
}

DailyPage::~DailyPage()
{
}

void DailyPage::handleQueryFinished(std::tuple<QString, QString, QString, QString> data)
{
    m_titleLabel->setText(std::get<0>(data));
    m_summaryLabel->setText(std::get<1>(data));

    QNetworkRequest request(QUrl(std::get<3>(data)));
    m_networkManager->get(request);

    connect(m_networkManager, &QNetworkAccessManager::finished, this, [=] (QNetworkReply *reply) {
        QByteArray imgData = reply->readAll();
        if (!imgData.isEmpty()) {
            QPixmap pixmap;
            pixmap.loadFromData(imgData);
            m_imageLabel->setPixmap(pixmap);
        }
    });
}