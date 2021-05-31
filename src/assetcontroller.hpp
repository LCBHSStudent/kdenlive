#ifndef ASSETCONTROLLER_H
#define ASSETCONTROLLER_H

class AssetParameterModel;
class EffectStackModel;

#include "definitions.h"
#include <memory>
#include <QSize>

/** @class AssetController
    @brief 用于设置项目资源的效果属性(filter/effect)和转场混合属性(transition/mix)
    开放接口至QtQuick Components供RSToolbar中的组件进行调控
 */
class AssetController : public QObject {
    Q_OBJECT

public:
	static std::unique_ptr<AssetController>& instance();
	Q_INVOKABLE ObjectId effectStackOwner();
	Q_INVOKABLE bool addEffect(const QString& effectId);
    Q_INVOKABLE bool selectSizePositionAdjust();
	
public slots:
	/** @brief 选中转场 */
    void selectTransition(int tid, const std::shared_ptr<AssetParameterModel> &transitionModel);
    /** @brief 选中混合 */
    void selectMix(int cid, const std::shared_ptr<AssetParameterModel> &mixModel);
    /** @brief 选择效果栈 */
    void selectEffectStack(const QString &itemName, const std::shared_ptr<EffectStackModel> &effectsModel, QSize frameSize, bool showKeyframes);
	/** @brief 如果 ${itemId} 与当前素材id相同 则清理数据 */
	void clearAssetData(int itemId);
	/** @brief 传递警告消息 */
	void assetControllerWarning(const QString service, const QString id, const QString message);
	
signals:
	void doSplitEffect(bool);
    void doSplitBinEffect(bool);
	void seekToPos(int);
	void reloadEffect(const QString &path);
    void switchCurrentComposition(int tid, const QString &compoId);

private:
	void clear();
	AssetController(QObject* parent = nullptr);
	inline static std::unique_ptr<AssetController> s_instance;
	
	std::shared_ptr<AssetParameterModel>	m_transModel	= { nullptr };
	std::shared_ptr<AssetParameterModel>	m_mixModel		= { nullptr };
	std::shared_ptr<EffectStackModel>		m_effectsModel	= { nullptr };
};

#endif
