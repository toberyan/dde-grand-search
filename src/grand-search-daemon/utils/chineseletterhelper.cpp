/*
 * Copyright (C) 2021 Uniontech Software Technology Co., Ltd.
 *
 * Author:     liuzhangjian<liuzhangjian@uniontech.com>
 *
 * Maintainer: liuzhangjian<liuzhangjian@uniontech.com>
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
#include "chineseletterhelper.h"

#include <QFile>
#include <QTextStream>

namespace GrandSearch {

class ChineseLetterHelperGlobal : public ChineseLetterHelper {};
Q_GLOBAL_STATIC(ChineseLetterHelperGlobal, chineseLetterHelperGlobal)

ChineseLetterHelper *ChineseLetterHelper::instance()
{
    return chineseLetterHelperGlobal;
}

ChineseLetterHelper::ChineseLetterHelper()
{

}

bool ChineseLetterHelper::chinese2Pinyin(const QString &words, QString &result)
{
    int ok = false;
    for (int i = 0; i < words.length(); ++i) {
        const uint key = words.at(i).unicode();
        auto found = m_dict.find(key);

        if (found != m_dict.end()) {
            result.append(found.value());
            ok = true;
        } else {
            result.append(words.at(i));
        }
    }

    return ok;
}

void ChineseLetterHelper::initDict()
{
    if (m_inited)
        return;

    m_inited = true;

    const QString dictPath = ":/misc/pinyin.dict";
    const int maxWord = 25333;
    QHash<uint, QString> dict;
    dict.reserve(maxWord);

    QFile file(dictPath);
    if (!file.open(QIODevice::ReadOnly))
        return;

    QByteArray content = file.readAll();
    file.close();

    QTextStream stream(&content, QIODevice::ReadOnly);
    while (!stream.atEnd()) {
        const QString line = stream.readLine();
        const QStringList items = line.split(QChar(':'));

        if (items.size() == 2) {
            dict.insert(static_cast<uint>(items[0].toInt(nullptr, 16)), items[1]);
        }
    }

    m_dict = dict;
}

bool ChineseLetterHelper::convertChinese2Pinyin(const QString &inStr, QString &outFirstPy, QString &outFullPy)
{
    if (inStr.isEmpty())
        return false;

    initDict();

    bool ok = false;
    for (int i = 0; i < inStr.size(); ++i) {
        const QString &cur = inStr.at(i);
        QString py;
        if (chinese2Pinyin(cur, py)) {
            ok = true;

            if (py.isEmpty())
                continue;

            outFirstPy.append(py.at(0));
            outFullPy.append(py);
        } else {
            outFirstPy.append(cur);
            outFullPy.append(cur);
        }
    }

    return ok;
}

}
