package com.adrestia

import com.badlogic.gdx.ScreenAdapter
import com.badlogic.gdx.scenes.scene2d.Stage

abstract class AdrestiaScreenAdaptor(protected val game: AdrestiaGame) : ScreenAdapter(), AdrestiaScreen {
    protected abstract val stage: Stage

    override fun touchUp(screenX: Int, screenY: Int, pointer: Int, button: Int): Boolean {
        return stage.touchUp(screenX, screenY, pointer, button)
    }

    override fun mouseMoved(screenX: Int, screenY: Int): Boolean {
        return stage.mouseMoved(screenX, screenY)
    }

    override fun keyTyped(character: Char): Boolean {
        return stage.keyTyped(character)
    }

    override fun scrolled(amount: Int): Boolean {
        return stage.scrolled(amount)
    }

    override fun keyUp(keycode: Int): Boolean {
        return stage.keyUp(keycode)
    }

    override fun touchDragged(screenX: Int, screenY: Int, pointer: Int): Boolean {
        return stage.touchDragged(screenX, screenY, pointer)
    }

    override fun keyDown(keycode: Int): Boolean {
        return stage.keyDown(keycode)
    }

    override fun touchDown(screenX: Int, screenY: Int, pointer: Int, button: Int): Boolean {
        return stage.touchDown(screenX, screenY, pointer, button)
    }
}