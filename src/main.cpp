#include <Geode/modify/LevelListLayer.hpp>

using geode::cocos::CCArrayExt;

struct
{
	int getFixedDemonInt(GJGameLevel* level)
	{
		switch (level->m_demonDifficulty)
		{
			case 3: return 1;
			case 4: return 2;
			case 5: return 9;
			case 6: return 10;
			default: return 6;
		}
	}

	bool operator()(GJGameLevel* left, GJGameLevel* right)
	{
		int lstars = left->m_stars.value();
		int rstars = right->m_stars.value();

		if (lstars == 10 && rstars == 10)
		{
			return getFixedDemonInt(left) < getFixedDemonInt(right);
		}

		lstars = (lstars == 0) ? getFixedDemonInt(left) : lstars;
		rstars = (rstars == 0) ? getFixedDemonInt(right) : rstars;

		return lstars < rstars;
	}
} sorter;

struct MyListLayer : geode::Modify<MyListLayer, LevelListLayer>
{
	void onSort(CCObject*)
	{
		std::vector<GJGameLevel*> newOrder;

		auto& list = this->m_levelList;
		auto levels = CCArrayExt<GJGameLevel*>(list->getListLevelsArray(nullptr));
		std::copy(levels.begin(), levels.end(), std::back_inserter(newOrder));

		std::sort(newOrder.begin(), newOrder.end(), sorter);
	
		for (int i = 0; GJGameLevel* l : newOrder)
		{
			this->m_levelList->reorderLevel(l->m_levelID.value(), i);
			i++;
		}

		this->onRefresh(nullptr);
	}

	$override bool init(GJLevelList* list)
	{
		LevelListLayer::init(list);

		if (auto node = getChildByID("right-side-menu"))
		{
			auto spr = geode::CircleButtonSprite::createWithSpriteFrameName("GJ_sortIcon_001.png");
			spr->setScale(.8f);

			auto btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(MyListLayer::onSort));

			node->addChild(btn);
			node->updateLayout();
		}

		return true;
	}
};