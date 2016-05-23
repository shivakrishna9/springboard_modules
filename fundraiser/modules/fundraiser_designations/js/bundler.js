(function ($) {
    Drupal.behaviors.fdBundler = {
        attach: function (context, settings) {
            $(window).ready(function () {
                window.App = {
                    Handlers: {}
                };
                App.Handlers.reorderItems = function () {

                    $('#fundraiser-designations-categories table').draggable({
                        connectToSortable: "#fundraiser-designations-groups",
                        helper: "clone",
                        revert: false,
                        placeholder: 'sortable-placeholder',
                        cursorAt: {left: 5},
                        stop: function (event, ui) {
                            App.Handlers.reorderItems();
                        }
                    });

                    $(".designations-category-table tr").draggable({
                        connectToSortable: "#fundraiser-designations-groups table",
                        helper: "clone",
                        revert: false,
                        cancel: 'th',
                        placeholder: 'sortable-placeholder',
                        cursorAt: {left: 5},
                        stop: function (event, ui) {
                            App.Handlers.reorderItems();
                        }
                    });

                    $("#fundraiser-designations-groups table tr").draggable({
                        connectToSortable: "#fundraiser-designations-groups table",
                        helper: "original",
                        revert: false,
                        cancel: 'th',
                        placeholder: 'sortable-placeholder',
                        cursorAt: {left: 5}
                    });


                    $('#fundraiser-designations-groups table').sortable({
                        cancel: 'th, thead',
                        placeholder: 'sortable-placeholder',
                        items: 'tr:not("thead tr")',
                        cursorAt: {left: 5},
                        start: function (event, ui) {
                        },
                        stop: function (event, ui) {
                            App.Handlers.reorderItems();
                        },
                    });

                    $('#fundraiser-designations-groups').sortable({
                        placeholder: 'sortable-placeholder',
                        items: 'table',
                        cursorAt: {left: 5},
                        start: function (event, ui) {
                        },
                        stop: function (event, ui) {
                            App.Handlers.reorderItems();
                        },
                    });
                }

                App.Handlers.reorderItems();
            }); // End window.ready
        }
    }
})(jQuery);
