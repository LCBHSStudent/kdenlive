#ifndef ASSETCONTROLLER_H
#define ASSETCONTROLLER_H

class AssetParameterModel;
class EffectStackModel;
class EffectItemModel;

#include "definitions.h"
#include <memory>
#include <QSize>
#include <QMutex>
#include <QPoint>

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
	/** @brief 获取过滤器参数 */
    QVariant getFilterParam(QString key) const;
    /** @brief 设置过滤器参数 */
    QVariant setFilterParam(QString key, QVariant value);
    
signals:
    /** @brief 设置激活首效果为当前效果 */
    void activateEffect(int row);
    /** @brief 从xml添加效果  */
    void addEffect(const QDomElement &e);
    /** @brief 向上/向下调整一级效果在效果栈中的层级 */
    void changeEffectPosition(const QList<int> &, bool upwards);
    /** @brief 创建效果组 */
    void createGroup(std::shared_ptr<EffectItemModel> effectModel);
    /** @brief 直接调整效果层级 */
    void moveEffect(const QList<int> &current_pos, int new_pos, int groupIndex, const QString &groupName);
    /** @brief 保存当前剪辑素材的整个效果栈 */
    void saveStack();
    /** @brief 效果保存后，触发效果列表重载 */
    void reloadEffects();
    /** @brief 里用本地路径重新加载效果栈 */
    void reloadEffect(const QString &path);
    /** @brief 删除指定效果项目 */
    void deleteEffect(std::shared_ptr<EffectItemModel> effect);
    /** @brief 刷新效果参数 */
    void refreshParams();
    /** @brief 分割时间线素材效果 */
	void doSplitEffect(bool);
    /** @brief 分割项目资源库效果 */
    void doSplitBinEffect(bool);
    /** @brief 定位到指定时间点 */
	void seekToPos(int);
    /** @brief 显示效果片段区域 */
    void showEffectZone(ObjectId id, QPair <int, int>inOut, bool checked);
    /** @brief 切换当前指定的混合/转场效果 */
    void switchCurrentComposition(int tid, const QString &compoId);

private:
    explicit AssetController(QObject* parent = nullptr);
    
    void clear();
    void clearAssetParameterModel(bool isTransitionModel);
    void clearEffectStackModel();
    
private slots:
    void slotEffectModelDataUpdated(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);
    void slotLoadEffectParameters();
    void slotSaveStack();
    void slotUpdateEffectZone(const QPoint p, bool withUndo);
    
private:
	inline static std::unique_ptr<AssetController> s_instance;
	
	std::shared_ptr<AssetParameterModel>	m_transModel	= { nullptr };
	std::shared_ptr<AssetParameterModel>	m_mixModel		= { nullptr };
	std::shared_ptr<EffectStackModel>		m_effectsModel	= { nullptr };
    
    QVariantMap m_filterKV = {};
    
    QMutex m_lock = {};
};

#endif
