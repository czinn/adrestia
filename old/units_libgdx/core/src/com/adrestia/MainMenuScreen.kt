package com.adrestia

import com.badlogic.gdx.Gdx
import com.badlogic.gdx.graphics.GL20
import com.badlogic.gdx.graphics.glutils.ShapeRenderer
import com.badlogic.gdx.scenes.scene2d.InputEvent
import com.badlogic.gdx.scenes.scene2d.Stage
import com.badlogic.gdx.scenes.scene2d.ui.Table
import com.badlogic.gdx.scenes.scene2d.ui.TextButton
import com.badlogic.gdx.scenes.scene2d.utils.ClickListener
import com.badlogic.gdx.utils.viewport.FitViewport

class MainMenuScreen(game: AdrestiaGame) : AdrestiaScreenAdaptor(game) {

    override val stage = Stage(FitViewport(400f, 600f))

    init {
        val table = Table()
        table.setFillParent(true)
        stage.addActor(table)

        val button = TextButton("0 bitcoins mined", game.skin)
        table.add(button)
        table.row()

        val settingsButton = TextButton("Settings", game.skin)
        table.add(settingsButton)

        var bitcoins = 0

        button.addListener {
            System.out.println(it)
            bitcoins += 1
            button.setText("$bitcoins bitcoins mined")
            true
        }

        settingsButton.addListener(object : ClickListener() {
            override fun clicked(event: InputEvent?, x: Float, y: Float) {
                super.clicked(event, x, y)
                game.pushScreen(SettingsScreen(game))
            }
        })
    }

    override fun render(delta: Float) {
        Gdx.gl.glClearColor(0.2f, 0.2f, 0.2f, 1f)
        Gdx.gl.glClear(GL20.GL_COLOR_BUFFER_BIT)

        stage.act(delta)
        stage.draw()
    }

    override fun resize(width: Int, height: Int) {
        stage.viewport.update(width, height, true)
    }

    override fun dispose() {
        stage.dispose()
    }
}