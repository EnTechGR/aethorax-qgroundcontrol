#include "AethoraxLabels.h"
#include "AethoraxConfig.h"

AethoraxLabels::AethoraxLabels(QObject *parent)
    : QTranslator(parent)
{
    const QVariantMap contexts = AethoraxConfig::section(QStringLiteral("labels"));
    for (auto context = contexts.cbegin(); context != contexts.cend(); ++context) {
        if (context.value().typeId() != QMetaType::QVariantMap) {
            continue;  // "comment"
        }
        const QVariantMap texts = context.value().toMap();
        for (auto text = texts.cbegin(); text != texts.cend(); ++text) {
            _labels[context.key()].insert(text.key(), text.value().toString());
        }
    }
}

QString AethoraxLabels::translate(const char *context, const char *sourceText, const char *disambiguation, int n) const
{
    Q_UNUSED(disambiguation);
    Q_UNUSED(n);
    const auto texts = _labels.constFind(QString::fromUtf8(context));
    if (texts == _labels.constEnd()) {
        return QString();
    }
    return texts->value(QString::fromUtf8(sourceText));
}
