package com.adrestia

import com.badlogic.gdx.*
import com.badlogic.gdx.graphics.OrthographicCamera
import com.badlogic.gdx.graphics.g2d.SpriteBatch
import com.badlogic.gdx.scenes.scene2d.ui.Skin
import java.util.*

class AdrestiaGame : Game() {
    var skin: Skin? = null
        private set

    private var camera: OrthographicCamera? = null
    private var batch: SpriteBatch? = null
    private var screenStack: Stack<AdrestiaScreen> = Stack()
    private val inputProcessor = InputMultiplexer(object : InputAdapter() {
        override fun keyDown(keycode: Int): Boolean {
            if (keycode == Input.Keys.BACK) {
                popScreen()
                return true
            }
            return false
        }
    })

    override fun create() {
        skin = Skin(Gdx.files.internal("ui_skin/skin/uiskin.json"))
        batch = SpriteBatch()
        camera = OrthographicCamera()

        Gdx.input.setCatchBackKey(true)
        Gdx.input.inputProcessor = inputProcessor

        pushScreen(MainMenuScreen(this))
    }

    override fun dispose() {
        while (!screenStack.empty()) {
            screenStack.pop().dispose()
        }
        batch!!.dispose()
        skin!!.dispose()
    }

    fun pushScreen(screen: AdrestiaScreen) {
        if (!screenStack.empty()) {
            inputProcessor.removeProcessor(screenStack.peek())
        }
        screenStack.push(screen)
        setScreen(screen)
        inputProcessor.addProcessor(screen)
    }

    fun popScreen() {
        val screen = screenStack.pop()
        screen.dispose()
        inputProcessor.removeProcessor(screen)

        if (screenStack.empty()) {
            Gdx.app.exit()
        } else {
            setScreen(screenStack.peek())
            inputProcessor.addProcessor(screenStack.peek())
        }
    }

    fun replaceScreen(screen: AdrestiaScreen) {
        screenStack.pop().dispose()
        pushScreen(screen)
    }

    // Other methods: resize, pause, resume, dispose
}
