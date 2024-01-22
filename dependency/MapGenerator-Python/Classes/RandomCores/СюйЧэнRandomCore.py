from __future__ import annotations
from math import floor
from random import random

from easygui import multenterbox

from Classes.MapStruct import MapStruct
from Classes.RandomCore import RandomCore


class DefaultСюйЧэнRandomSettings:
    asteroidWidth = 2
    resourceNum = 7
    constructionNum = 5
    shadowProb = 0.015
    shadowCrossBonus = 23
    ruinProb = 0.01
    ruinCrossBonus = 40


class СюйЧэнRandomCore(RandomCore):
    title: str
    asteroidWidth: int
    resourceNum: int
    constructionNum: int
    shadowProb: float
    shadowCrossBonus: int
    ruinProb: float
    ruinCrossBonus: int

    @property
    def AsteroidWidth(self) -> int:
        return self.asteroidWidth

    @AsteroidWidth.setter
    def AsteroidWidth(self, value: int) -> None:
        if value < 1 or value > 4:
            self.asteroidWidth = DefaultСюйЧэнRandomSettings.asteroidWidth
        else:
            self.asteroidWidth = value

    @property
    def ResourceNum(self) -> int:
        return self.resourceNum

    @ResourceNum.setter
    def ResourceNum(self, value: int) -> None:
        if value < 1 or value > 10:
            self.resourceNum = DefaultСюйЧэнRandomSettings.resourceNum
        else:
            self.resourceNum = value

    @property
    def ConstructionNum(self) -> int:
        return self.constructionNum

    @ConstructionNum.setter
    def ConstructionNum(self, value: int) -> None:
        if value < 1 or value > 10:
            self.constructionNum = DefaultСюйЧэнRandomSettings.constructionNum
        else:
            self.constructionNum = value

    @property
    def ShadowProb(self) -> float:
        return self.shadowProb

    @ShadowProb.setter
    def ShadowProb(self, value: float) -> None:
        if value < 0 or value > 0.1:
            self.shadowProb = DefaultСюйЧэнRandomSettings.shadowProb
        else:
            self.shadowProb = value

    @property
    def ShadowCrossBonus(self) -> int:
        return self.shadowCrossBonus

    @ShadowCrossBonus.setter
    def ShadowCrossBonus(self, value: int) -> None:
        if value < 1 or value > 50:
            self.shadowCrossBonus = DefaultСюйЧэнRandomSettings.shadowCrossBonus
        else:
            self.shadowCrossBonus = value

    @property
    def RuinProb(self) -> float:
        return self.ruinProb

    @RuinProb.setter
    def RuinProb(self, value: float) -> None:
        if value < 0 or value > 0.1:
            self.ruinProb = DefaultСюйЧэнRandomSettings.ruinProb
        else:
            self.ruinProb = value

    @property
    def RuinCrossBonus(self) -> int:
        return self.ruinCrossBonus

    @RuinCrossBonus.setter
    def RuinCrossBonus(self, value: int) -> None:
        if value < 1 or value > 50:
            self.ruinCrossBonus = DefaultСюйЧэнRandomSettings.ruinCrossBonus
        else:
            self.ruinCrossBonus = value

    def __init__(self,
                 title,
                 asteroidWidth: int = DefaultСюйЧэнRandomSettings.asteroidWidth,
                 resourceNum: int = DefaultСюйЧэнRandomSettings.resourceNum,
                 constructionNum: int = DefaultСюйЧэнRandomSettings.constructionNum,
                 shadowProb: float = DefaultСюйЧэнRandomSettings.shadowProb,
                 shadowCrossBonus: int = DefaultСюйЧэнRandomSettings.shadowCrossBonus,
                 ruinProb: float = DefaultСюйЧэнRandomSettings.ruinProb,
                 ruinCrossBonus: int = DefaultСюйЧэнRandomSettings.ruinCrossBonus) -> None:
        self.title = title
        self.AsteroidWidth = asteroidWidth
        self.ResourceNum = resourceNum
        self.ConstructionNum = constructionNum
        self.ShadowProb = shadowProb
        self.ShadowCrossBonus = shadowCrossBonus
        self.RuinProb = ruinProb
        self.RuinCrossBonus = ruinCrossBonus

    @property
    def Name(self) -> str:
        return 'СюйЧэн'

    def Menu(self) -> bool:
        try:
            (self.AsteroidWidth,
             self.ResourceNum,
             self.ConstructionNum,
             self.ShadowProb,
             self.ShadowCrossBonus,
             self.RuinProb,
             self.RuinCrossBonus) = (lambda i1, i2, i3, f4, i5, f6, i7:
                                     (int(i1), int(i2), int(i3), float(f4), int(i5), float(f6), int(i7)))(*multenterbox(
                                         msg='Random settings',
                                         title=self.title,
                                         fields=[
                                             'Asteroid 宽度',
                                             'Resource 数量',
                                             'Construction 数量',
                                             'Shadow 生成概率',
                                             'Shadow 蔓延加成',
                                             'Ruin 生成概率',
                                             'Ruin 蔓延加成'
                                         ],
                                         values=[self.AsteroidWidth,
                                                 self.ResourceNum,
                                                 self.ConstructionNum,
                                                 self.ShadowProb,
                                                 self.ShadowCrossBonus,
                                                 self.RuinProb,
                                                 self.RuinCrossBonus]
                                     ))
        except TypeError:
            return False
        return True

    def Random(self, mp: MapStruct) -> None:
        mp.Clear()
        СюйЧэнRandomCore.generateBorderRuin(mp)
        СюйЧэнRandomCore.generateHome(mp)
        СюйЧэнRandomCore.generateAsteroid(mp, self.asteroidWidth)
        СюйЧэнRandomCore.generateResource(mp, self.resourceNum)
        СюйЧэнRandomCore.generateConstruction(mp, self.constructionNum)
        СюйЧэнRandomCore.generateShadow(mp, self.shadowProb, self.shadowCrossBonus)
        СюйЧэнRandomCore.generateRuin(mp, self.ruinProb, self.ruinCrossBonus)
        СюйЧэнRandomCore.generateWormhole(mp)

    @staticmethod
    def isEmptyNearby(mp: MapStruct, x: int, y: int, r: int) -> bool:
        for i in range(x - r if x - r >= 0 else 0, (x + r if x + r <= 49 else 49) + 1):
            for j in range(y - r if y - r >= 0 else 0, (y - r if y + r <= 9 else 49) + 1):
                if mp[i, j] != 0:
                    return False
        return True

    @staticmethod
    def haveSthNearby(mp: MapStruct, x: int, y: int, r: int, tp: int) -> int:
        ret = 0
        for i in range(x - r if x - r >= 0 else 0, (x + r if x + r <= 49 else 49) + 1):
            for j in range(y - r if y - r >= 0 else 0, (y - r if y + r <= 9 else 49) + 1):
                if mp[i, j] == tp:
                    ret += 1
        return ret

    @staticmethod
    def haveSthCross(mp: MapStruct, x: int, y: int, r: int, tp: int) -> int:
        ret = 0
        for i in range(x - r if x - r >= 0 else 0, (x + r if x + r <= 49 else 49) + 1):
            if mp[i, y] == tp:
                ret += 1
        for j in range(y - r if y - r >= 0 else 0, (y + r if y + r <= 49 else 49) + 1):
            if mp[x, j] == tp:
                ret += 1
        return ret

    @staticmethod
    def generateBorderRuin(mp: MapStruct) -> None:
        for i in range(50):
            mp[i, 0] = 1
            mp[i, 49] = 1
            mp[0, i] = 1
            mp[49, i] = 1

    @staticmethod
    def generateHome(mp: MapStruct) -> None:
        mp[3, 46] = 7
        mp[46, 3] = 7

    @staticmethod
    def generateAsteroid(mp: MapStruct, width: int = DefaultСюйЧэнRandomSettings.asteroidWidth) -> None:
        for i in range(1, 49):
            for j in range(24, 24 - width, -1):
                mp[i, j] = 3
                mp[49 - i, 49 - j] = 3
        for i in range(1, 23):
            if random() < 0.5 and i != 9 and i != 10 and i != 11 and i != 12:
                mp[i, 24 - width] = 3
                mp[i, 24 + width] = 0
                mp[49 - i, 25 + width] = 3
                mp[49 - i, 25 - width] = 0

    @staticmethod
    def generateResource(mp: MapStruct, num: int = DefaultСюйЧэнRandomSettings.resourceNum) -> None:
        i = 0
        while i < num:
            x = floor(random() * 48) + 1
            y = floor(random() * 23) + 1
            if СюйЧэнRandomCore.isEmptyNearby(mp, x, y, 2):
                mp[x, y] = 4
                mp[49 - x, 49 - y] = 4
            else:
                i -= 1
            i += 1

    @staticmethod
    def generateConstruction(mp: MapStruct, num: int = DefaultСюйЧэнRandomSettings.constructionNum) -> None:
        i = 0
        while i < num:
            x = floor(random() * 48) + 1
            y = floor(random() * 23) + 1
            if СюйЧэнRandomCore.isEmptyNearby(mp, x, y, 1):
                mp[x, y] = 5
                mp[49 - x, 49 - y] = 5
            else:
                i -= 1
            i += 1

    @staticmethod
    def generateShadow(mp: MapStruct, prob: float = DefaultСюйЧэнRandomSettings.shadowProb,
                       crossBonus: int = DefaultСюйЧэнRandomSettings.shadowCrossBonus) -> None:
        for i in range(50):
            for j in range(50):
                if (mp[i, j] == 0 and
                        random() < prob * (СюйЧэнRandomCore.haveSthCross(mp, i, j, 1, 2) * crossBonus + 1)):
                    mp[i, j] = 2
                    mp[49 - i, 49 - j] = 2

    @staticmethod
    def generateRuin(mp: MapStruct, prob: float = DefaultСюйЧэнRandomSettings.ruinProb,
                     crossBonus: int = DefaultСюйЧэнRandomSettings.ruinCrossBonus) -> None:
        for i in range(2, 48):
            for j in range(2, 48):
                if ((mp[i, j] == 0 or mp[i, j] == 2) and
                    not СюйЧэнRandomCore.haveSthNearby(mp, i, j, 1, 3) and
                    not СюйЧэнRandomCore.haveSthNearby(mp, i, j, 1, 7) and
                        random() < prob
                        * (СюйЧэнRandomCore.haveSthCross(mp, i, j, 1, 1)
                           * (0 if СюйЧэнRandomCore.haveSthCross(mp, i, j, 1, 1) > 1
                              else crossBonus) + 1)):
                    mp[i, j] = 1
                    mp[49 - i, 49 - j] = 1

    @staticmethod
    def generateWormhole(mp: MapStruct) -> None:
        for i in range(1, 49):
            if mp[10, i] == 3:
                mp[10, i] = 6
                mp[39, 49 - i] = 6
            if mp[11, i] == 3:
                mp[11, i] = 6
                mp[38, 49 - i] = 6
            if mp[24, i] == 3:
                mp[24, i] = 6
                mp[25, 49 - i] = 6
