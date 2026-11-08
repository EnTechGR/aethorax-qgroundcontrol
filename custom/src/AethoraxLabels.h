#pragma once

#include <QtCore/QHash>
#include <QtCore/QString>
#include <QtCore/QTranslator>

/// The application's text, renamed from aethorax.json's "labels": for each context (the C++
/// class or the QML file's name, e.g. "PlanView"), the original English text and ours.
/// Installed as a translator, so it renames text wherever upstream shows it, without
/// replacing the file. A language translation of the same text, if installed later, wins.
class AethoraxLabels : public QTranslator
{
    Q_OBJECT

public:
    explicit AethoraxLabels(QObject *parent = nullptr);

    QString translate(const char *context, const char *sourceText, const char *disambiguation = nullptr, int n = -1) const final;
    bool isEmpty() const final { return _labels.isEmpty(); }

private:
    QHash<QString, QHash<QString, QString>> _labels;
};
