package org.graphgui;

import javafx.application.Application;
import javafx.geometry.Insets;
import javafx.scene.Scene;
import javafx.scene.control.*;
import javafx.scene.input.KeyEvent;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;
import javafx.scene.layout.StackPane;
import javafx.stage.FileChooser;
import javafx.stage.Stage;
import javafx.scene.paint.Color;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;


public class HelloApplication extends Application {
    private String outputFilePath="../results.txt";//path to file that stores results of division
    private Stage primaryStage;
    private File chosenFile;
    private InputMode currentMode;

    private RadioButton fileButton;
    private RadioButton testsButton;
    private RadioButton enterManuallyButton;
    private ToggleGroup grupa1;

    private TextField nodesInput;

    private TextArea divideResults;//stores results of division

    private TextArea connectionsInput;

    private TextField partitionsInput;
    private TextField precisionInput;
    private VBox additionalInputs;

    private Button mainConfirmButton;
    private Button detailedConfirmButton;
    private Button backButton;

    private VBox dynamicContent;

    private HBox confirmBackButtonsPlacement=createConfirmBackButtons();

    @Override
    public void start(Stage stage) {
        this.primaryStage = stage;
        VBox buttonsPlacement = initStartMenu();

        detailedConfirmButton = new Button("Confirm");
        detailedConfirmButton.getStyleClass().add("confirm-button");

        backButton = new Button("Back");
        backButton.getStyleClass().add("back-button");

        // Main dynamic content container for switching modes
        dynamicContent = new VBox(15, buttonsPlacement);
        dynamicContent.setPadding(new Insets(20));
        dynamicContent.getStyleClass().add("root-pane");
        dynamicContent.setFillWidth(true);

        StackPane root = new StackPane(dynamicContent);

        additionalInputs = initSharedInputs();


        backButton.setOnAction(e -> {
            // Handle return to start menu
            dynamicContent.getChildren().clear();
            dynamicContent.getChildren().add(buttonsPlacement);
            primaryStage.sizeToScene();
        });

        Scene scene = new Scene(root);
        scene.setFill(Color.web("#1e1e2f"));
        stage.setScene(scene);

        stage.sizeToScene();

        scene.getStylesheets().add(getClass().getResource("/style.css").toExternalForm());

        stage.setTitle("Import Data");
        stage.setMinWidth(350);
        stage.setMinHeight(300);
        stage.show();
    }

    private VBox initRadioButtons() {//Initializes basic GUI look
        fileButton = new RadioButton("File");
        testsButton = new RadioButton("Basic tests");
        enterManuallyButton = new RadioButton("Enter manually");

        grupa1 = new ToggleGroup();
        fileButton.setToggleGroup(grupa1);
        testsButton.setToggleGroup(grupa1);
        enterManuallyButton.setToggleGroup(grupa1);

        Label importLabel = new Label("Import From: ");

        VBox radioButtonsPlacement = new VBox(10);
        radioButtonsPlacement.setPadding(new Insets(15));
        radioButtonsPlacement.getChildren().addAll(importLabel, fileButton, testsButton, enterManuallyButton);

        return radioButtonsPlacement;
    }

    private VBox initSharedInputs() {//Initializes input fields for number of partitions and precision
        Label partitionsLabel = new Label("Number of partitions:");
        partitionsInput = new TextField();
        partitionsInput.setPromptText("e.g. 3");
        partitionsInput.addEventFilter(KeyEvent.KEY_TYPED, event -> {
            if (!event.getCharacter().matches("\\d")) {
                event.consume();
            }
        });
        partitionsInput.getStyleClass().add("input-field");

        Label precisionLabel = new Label("Precision (0 < p < 1):");
        precisionInput = new TextField();
        precisionInput.setPromptText("e.g. 0.3");
        precisionInput.addEventFilter(KeyEvent.KEY_TYPED, event -> {
            if (!event.getCharacter().matches("[0-9\\.]")) {
                event.consume();
            }
        });
        precisionInput.getStyleClass().add("input-field");

        VBox additionalInputs = new VBox(10, partitionsLabel, partitionsInput, precisionLabel, precisionInput);
        additionalInputs.setPadding(new Insets(10));
        return additionalInputs;
    }

    // Initializes the file input section for .csrrg files
    private VBox initFileSection(Stage stage) {
        Button chooseFileButton = new Button("Choose .csrrg file");
        chooseFileButton.getStyleClass().add("file-button");

        chooseFileButton.setOnAction(e -> {
            FileChooser fileChooser = new FileChooser();
            FileChooser.ExtensionFilter csrrgFilter = new FileChooser.ExtensionFilter("CSRRG Files (*.csrrg)", "*.csrrg");
            fileChooser.getExtensionFilters().add(csrrgFilter);
            chosenFile = fileChooser.showOpenDialog(stage);
        });

        VBox wrapper = new VBox(15,
                new Label("Choose test file"),
                chooseFileButton,
                additionalInputs,
                confirmBackButtonsPlacement);
        wrapper.setPadding(new Insets(20));
        return wrapper;
    }

    // Initializes the manual graph input section
    private VBox initManualInputSection() {
        Label nodes = new Label("Number of nodes");
        TextField nodesInput = new TextField();
        nodesInput.setPromptText("e.g. 12");
        nodesInput.addEventFilter(KeyEvent.KEY_TYPED, event -> {
            if (!event.getCharacter().matches("\\d")) {
                event.consume();
            }
        });
        nodesInput.getStyleClass().add("input-field");
        this.nodesInput = nodesInput;
        VBox nodesLayout = new VBox(6, nodes, nodesInput);
        nodesLayout.setPadding(new Insets(15));

        Label connections = new Label("Graph connections (e.g. 1 2)");
        connectionsInput = new TextArea();
        connectionsInput.setPromptText("1 2");
        connectionsInput.setPrefRowCount(6);
        connectionsInput.textProperty().addListener((obs, oldText, newText) -> {
            if (!newText.matches("([0-9]+\\s+[0-9]+\\s*\\n?)*")) {
                connectionsInput.setStyle("-fx-border-color: red;");
            } else {
                connectionsInput.setStyle("");
            }
        });
        connectionsInput.getStyleClass().add("textarea-field");
        VBox connctionsLayout = new VBox(6, connections, connectionsInput);
        connctionsLayout.setPadding(new Insets(10));

        VBox manualInputSection = new VBox(12, nodesLayout, connctionsLayout, additionalInputs, confirmBackButtonsPlacement);
        manualInputSection.setPadding(new Insets(20));


        VBox wrapper = new VBox(15);
        wrapper.getChildren().addAll(
                // Placeholder label for future changes (to be updated)
                new Label("To change"),
                nodesLayout,
                connctionsLayout,
                additionalInputs,
                confirmBackButtonsPlacement
        );
        wrapper.setPadding(new Insets(20));
        return wrapper;
    }

    // Initializes the section for selecting basic test graphs
    private VBox initTestSelectionLayout() {
        RadioButton test1 = new RadioButton("Basic test 12 nodes, 3 columns and 4 rows");
        RadioButton test2 = new RadioButton("Creates irregular graph with 12 nodes");
        RadioButton test3 = new RadioButton("Hybrid custom-random graph");

        ToggleGroup grupa2 = new ToggleGroup();
        test1.setToggleGroup(grupa2);
        test2.setToggleGroup(grupa2);
        test3.setToggleGroup(grupa2);

        VBox testyLayout = new VBox(15, test1, test2, test3);
        testyLayout.setPadding(new Insets(10));

        VBox wrapper = new VBox(15);
        wrapper.getChildren().addAll(
            new Label("Choose test option"),
                testyLayout,
                additionalInputs, // recreate shared inputs dynamically
                confirmBackButtonsPlacement
        );
        wrapper.setPadding(new Insets(20));
        return wrapper;
    }

    // Creates the confirm and back buttons for detailed input sections
    private HBox createConfirmBackButtons() {
        detailedConfirmButton = new Button("Confirm");
        detailedConfirmButton.getStyleClass().add("confirm-button");
        setupDetailedConfirmHandler();

        Button back = new Button("Back");
        back.getStyleClass().add("back-button");
        back.setOnAction(e -> {
            dynamicContent.getChildren().clear();
            dynamicContent.getChildren().add(initStartMenu());
            primaryStage.sizeToScene();
        });

        HBox buttonBox = new HBox(60, back, detailedConfirmButton);
        buttonBox.setPadding(new Insets(15));
        return buttonBox;
    }

    // Initializes the start menu with import options
    private VBox initStartMenu() {
        VBox radioButtonsPlacement = initRadioButtons();
        mainConfirmButton = new Button("Confirm");
        mainConfirmButton.getStyleClass().add("confirm-button");
        setupMainConfirmHandler();

        VBox buttonsPlacement = new VBox(15, radioButtonsPlacement, mainConfirmButton);
        buttonsPlacement.setPadding(new Insets(20));
        return buttonsPlacement;
    }

    // Handles dynamic content switching between modes
    private void setupMainConfirmHandler() {
        mainConfirmButton.setOnAction(e -> {
            Toggle selected = grupa1.getSelectedToggle();
            dynamicContent.getChildren().clear();

            if (selected == fileButton) {
                currentMode=InputMode.FILE;
                VBox fileSection = initFileSection(primaryStage);
                dynamicContent.getChildren().add(fileSection);
                primaryStage.sizeToScene();
            } else if (selected == testsButton) {
                currentMode=InputMode.TEST;
                VBox testsSection = initTestSelectionLayout();
                dynamicContent.getChildren().add(testsSection);
                primaryStage.sizeToScene();
            } else if (selected == enterManuallyButton) {
                currentMode=InputMode.MANUAL;
                VBox manualSection = initManualInputSection();
                dynamicContent.getChildren().add(manualSection);
                primaryStage.sizeToScene();
            }
        });
    }

    // Processes confirmation button action and graph division logic
    private void setupDetailedConfirmHandler() {
        detailedConfirmButton.setOnAction(e -> {
            boolean valid = true;

            String partitionsText = partitionsInput.getText();
            String precisionText = precisionInput.getText();

            int partitions = -1;
            double precision = -1;

            try {
                partitions = Integer.parseInt(partitionsText);
            } catch (NumberFormatException ex) {
                valid = false;
                System.out.println("Partitions must be an integer.");
            }

            try {
                precision = Double.parseDouble(precisionText);
            } catch (NumberFormatException ex) {
                valid = false;
                System.out.println("Precision must be a number.");
            }

            if (precision <= 0 || precision >= 1) {
                valid = false;
                System.out.println("Precision must be in (0, 1).");
            }

            switch (currentMode) {//evokes adequate (to chosen option) partitioning function
                case FILE -> {
                    if (chosenFile != null) {
                        System.out.println("Chosen file: " + chosenFile.getAbsolutePath());
                        GraphLib.INSTANCE.fromFile(chosenFile.getAbsolutePath(),partitions,precision,outputFilePath);
                    } else {
                        valid = false;
                        System.out.println("No file selected.");
                    }
                }
                case MANUAL -> {
                    int nodesCount = -1;
                    try {
                        nodesCount = Integer.parseInt(nodesInput.getText());
                    } catch (NumberFormatException ex) {
                        valid = false;
                        System.out.println("Invalid node count.");
                    }

                    if (nodesCount > 0 && partitions > nodesCount) {
                        valid = false;
                        System.out.println("Partitions cannot exceed number of nodes.");
                    }

                    System.out.println("Number of vertices: " + nodesInput.getText());
                    System.out.println(connectionsInput.getText());
                    String manualInputFile="data/custom-input.txt";
                    try(FileWriter fileWriter=new FileWriter(manualInputFile)){
                        fileWriter.write(nodesCount+"\n");
                        fileWriter.write(connectionsInput.getText());
                    }catch(IOException exception){
                        System.out.println("Error occurred: "+exception.getMessage());
                    }
                    GraphLib.INSTANCE.fromUserInput(manualInputFile,partitions,precision,outputFilePath);
                }
                case TEST -> {
                    System.out.println("Test selected");
                    GraphLib.INSTANCE.fromTests(currentMode.ordinal(),100,0.01,partitions,precision,outputFilePath);
                }
            }
            //Adds partition results to UI
            divideResults=new TextArea();
            divideResults.setEditable(false);
            divideResults.setText(FileUtils.fileToString(outputFilePath));
            divideResults.getStyleClass().add("output-field");
            dynamicContent.getChildren().add(divideResults);
            primaryStage.sizeToScene();
        });
    }


    public static void main(String[] args) {
        System.setProperty("jna.library.path", "lib");
        launch(args);
    }
}